
#include "PitchShifter.h"
#include <cmath>

PitchShifter::PitchShifter()
{
    pitchRatioSmoothed.reset(44100, 0.05);   // default; updated in prepare
    setWindowSizeMs(40.0);
    setOverlap(0.5);
    setSmoothingTimeMs(50.0);
    setSemitones(0.0f);
    setMix(1.0f);
}

void PitchShifter::prepare(double sampleRate, int maxBlockSize, int numChannels)
{
    sr = sampleRate;
    maxBlock = maxBlockSize;
    numCh = juce::jmax(1, numChannels);

    pitchRatioSmoothed.reset(sr, (float)(pitchRatioSmoothed.getRampDurationSeconds()));
    pitchRatioSmoothed.setCurrentAndTargetValue(targetRatio);

    mixSmoothed.reset(sr, (float)(mixSmoothed.getRampDurationSeconds()));
    mixSmoothed.setCurrentAndTargetValue(targetMix);

    updateDerived();
    allocateBuffers();
    reset();
}

void PitchShifter::reset()
{
    for (auto& ch : channels)
    {
        ch.delayBuffer.clear();
        ch.dryDelayBuffer.clear();

        ch.writeIndex = 0;
        ch.dryWriteIndex = 0;

        ch.g1.pos = ch.g2.pos = 0.0;
        ch.g1.startIndex = ch.g2.startIndex = 0;
    }
}

void PitchShifter::setSemitones(float semitones)
{
    // ratio = 2^(st/12)
    setPitchRatio(std::pow(2.0, semitones / 12.0));
}

void PitchShifter::setPitchRatio(double ratio)
{
    targetRatio = juce::jlimit(0.25, 4.0, ratio); // constrain to sane range = {-24st, +24st}
    pitchRatioSmoothed.setTargetValue(targetRatio);
}

void PitchShifter::setWindowSizeMs(double ms)
{
    windowSizeMs = juce::jlimit(10.0, 80.0, ms); // typical safe range
}

void PitchShifter::setOverlap(double fractional)
{
    overlap = juce::jlimit(0.33, 0.75, fractional); // 33–75%
}

void PitchShifter::setRetriggerDelayMs(double ms)
{
    retriggerDelayMs = ms; // -1 => auto (window size)
}

void PitchShifter::setSmoothingTimeMs(double ms)
{
    const double seconds = juce::jlimit(5.0, 2000.0, ms) / 1000.0;
    pitchRatioSmoothed.reset(sr, (float)seconds);
    mixSmoothed.reset(sr, (float)seconds);
}

void PitchShifter::setMix(float mix)
{
    targetMix = juce::jlimit(0.0f, 1.0f, mix);
    mixSmoothed.setTargetValue(targetMix);
}

void PitchShifter::updateDerived()
{
    windowSize = (int)std::round(sr * (windowSizeMs / 1000.0));
    windowSize = juce::jlimit(64, 8192, windowSize);

    hopSize = (int)std::round(windowSize * (1.0 - overlap));
    hopSize = juce::jmax(1, hopSize);

    retriggerDelay = (retriggerDelayMs > 0.0)
        ? (int)std::round(sr * (retriggerDelayMs / 1000.0))
        : windowSize;

    // Allocate at least a few windows worth to be safe
    delayBufferSize = juce::jmax(4 * windowSize, maxBlock * 4);

    // Latency ~ one window (worst case). Report safely.
    latencySamples = windowSize;

    // Precompute Hann table for [0..windowSize]
    hannTableSize = windowSize + 1;
    hannTable.allocate(hannTableSize, true);
    for (int n = 0; n < hannTableSize; ++n)
    {
        const double x = (double)n / (double)windowSize; // 0..1
        // Hann: 0.5 - 0.5 cos(2πx)
        hannTable[n] = (float)(0.5 - 0.5 * std::cos(juce::MathConstants<double>::twoPi * x));
    }
}

void PitchShifter::allocateBuffers()
{
    channels.resize(numCh);

    for (auto& ch : channels)
    {
        ch.delayBuffer.setSize(1, delayBufferSize, false, true, true);
        ch.delayBuffer.clear();
        ch.writeIndex = 0;

        const int drySize = juce::jmax(latencySamples + maxBlock, 2 * maxBlock);
        ch.dryDelayBuffer.setSize(1, drySize, false, true, true);
        ch.dryDelayBuffer.clear();
        ch.dryWriteIndex = 0;

        ch.g1.pos = ch.g2.pos = 0.0;
        ch.g1.startIndex = ch.g2.startIndex = 0;
    }
}

inline float PitchShifter::hannAt(double x01) const noexcept
{
    if (x01 <= 0.0) return 0.0f;
    if (x01 >= 1.0) return 0.0f;

    const double idx = x01 * (double)windowSize;
    const int i = (int)idx;
    const double frac = idx - i;

    const float a = hannTable[juce::jlimit(0, hannTableSize - 1, i)];
    const float b = hannTable[juce::jlimit(0, hannTableSize - 1, i + 1)];
    return a + (float)frac * (b - a);
}

inline float PitchShifter::readInterpolated(const ChannelState& ch, int /*channel*/, double readIndex) const noexcept
{
    // readIndex is in circular buffer space [0..delayBufferSize)
    const int i0 = (int)std::floor(readIndex);
    const int i1 = (i0 + 1) % delayBufferSize;
    const float frac = (float)(readIndex - i0);

    const float* d = ch.delayBuffer.getReadPointer(0);
    const float s0 = d[i0];
    const float s1 = d[i1];
    return s0 + frac * (s1 - s0);
}

void PitchShifter::retriggerGrain(ChannelState& ch, Grain& g, int delaySamples)
{
    // Start the new grain 'delaySamples' behind the current writeIndex
    int start = ch.writeIndex - delaySamples;
    while (start < 0) start += delayBufferSize;
    start %= delayBufferSize;

    g.startIndex = start;
    g.pos = 0.0;
}

void PitchShifter::process(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    if (numCh <= 0 || windowSize <= 0 || delayBufferSize <= 0)
        return;

    // Bypass when ratio ~ 1.0 to avoid coloration
    const double currentRatio = pitchRatioSmoothed.getNextValue();
    const float mix = mixSmoothed.getNextValue();
    const float dryGain = 1.0f - mix;
    const float wetGain = mix;

    const bool bypass = std::abs(currentRatio - 1.0) < epsilonBypass;

    // Initialize grains on first run
    for (auto& ch : channels)
    {
        if (ch.g1.pos == 0.0 && ch.g1.startIndex == 0)
            retriggerGrain(ch, ch.g1, retriggerDelay);
        if (ch.g2.pos == 0.0 && ch.g2.startIndex == 0)
            retriggerGrain(ch, ch.g2, retriggerDelay + hopSize);
    }

    // Process per channel
    for (int chIndex = 0; chIndex < buffer.getNumChannels(); ++chIndex)
    {
        auto& ch = channels[chIndex];
        float* out = buffer.getWritePointer(chIndex);
        const float* in = buffer.getReadPointer(chIndex);

        float* wetDelayWrite = ch.delayBuffer.getWritePointer(0);
        float* dryDelayWrite = ch.dryDelayBuffer.getWritePointer(0);
        const int drySize = ch.dryDelayBuffer.getNumSamples();

        for (int n = 0; n < numSamples; ++n)
        {
            // --- Push input into both buffers ---
            wetDelayWrite[ch.writeIndex] = in[n];
            dryDelayWrite[ch.dryWriteIndex] = in[n];
            
            // Compute dry (latency-compensated) sample: read back latencySamples behind write
            int dryReadIndex = ch.dryWriteIndex - latencySamples;
            while (dryReadIndex < 0) dryReadIndex += drySize;
            dryReadIndex %= drySize;
            const float drySample = ch.dryDelayBuffer.getReadPointer(0)[dryReadIndex];

            // --- Wet path ---
            float wetSample = 0.0f;

            if (!bypass)
            {
                const double inc = currentRatio;

                // Grain 1
                double g1Phase01 = ch.g1.pos / (double)windowSize;
                double g1ReadIndex = (double)ch.g1.startIndex + ch.g1.pos * inc;
                while (g1ReadIndex >= delayBufferSize) g1ReadIndex -= delayBufferSize;
                while (g1ReadIndex < 0.0) g1ReadIndex += delayBufferSize;

                const float g1Win = hannAt(g1Phase01);
                const float g1Samp = readInterpolated(ch, chIndex, g1ReadIndex) * g1Win;

                // Grain 2
                double g2Phase01 = ch.g2.pos / (double)windowSize;
                double g2ReadIndex = (double)ch.g2.startIndex + ch.g2.pos * inc;
                while (g2ReadIndex >= delayBufferSize) g2ReadIndex -= delayBufferSize;
                while (g2ReadIndex < 0.0) g2ReadIndex += delayBufferSize;

                const float g2Win = hannAt(g2Phase01);
                const float g2Samp = readInterpolated(ch, chIndex, g2ReadIndex) * g2Win;

                wetSample = g1Samp + g2Samp;

                // Advance grains
                ch.g1.pos += 1.0;
                ch.g2.pos += 1.0;

                if (ch.g1.pos >= (double)windowSize)
                    retriggerGrain(ch, ch.g1, retriggerDelay);
                if (ch.g2.pos >= (double)windowSize)
                    retriggerGrain(ch, ch.g2, retriggerDelay + hopSize);
            }
            else
            {
                // If bypassing (ratio ~ 1), treat wet as zero to avoid coloration,
                // i.e., output becomes latency-aligned dry only.
                wetSample = 0.0f;
            }

            // --- Mix and write output ---
            out[n] = dryGain * drySample + wetGain * wetSample;

            // Advance write pointers
            ch.writeIndex = (ch.writeIndex + 1) % delayBufferSize;
            ch.dryWriteIndex = (ch.dryWriteIndex + 1) % drySize;

        }
    }
}
