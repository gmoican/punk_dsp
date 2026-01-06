
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>

/**
 * Wavefolder processor (header-only).
 *
 * Features:
 *  - Drive (dB) with multiplicative smoothing
 *  - Threshold (fold limit, linear amplitude)
 *  - Symmetry (-1 .. +1): adjusts positive vs. negative fold limits
 *  - Bias (pre-fold DC offset)
 *  - Stages: cascaded folders (1..N)
 *  - Dry/Wet mix
 *  - Output gain (dB)
 *
 * Usage:
 *    Wavefolder wf;
 *    wf.prepare ({ sampleRate, (uint32) blockSize, (uint32) numChannels });
 *    wf.setDriveDecibels (18.0f);
 *    wf.setThreshold (0.5f);
 *    wf.setSymmetry (0.2f);
 *    wf.setBias (0.0f);
 *    wf.setStages (4);
 *    wf.setMix (0.8f);
 *    wf.setOutputGainDecibels (0.0f);
 *    wf.process (juce::dsp::AudioBlock<float> (buffer));
 */
class Wavefolder
{
public:
    Wavefolder() = default;

    //==========================================================================
    void prepare (const juce::dsp::ProcessSpec& spec)
    {
        jassert (spec.sampleRate > 0);
        jassert (spec.maximumBlockSize > 0);
        jassert (spec.numChannels > 0);

        sampleRate       = spec.sampleRate;
        maxBlockSize     = (int) spec.maximumBlockSize;
        numChannels      = (int) spec.numChannels;

        // Smoothing setup (time constants ~ 30ms)
        constexpr double smoothTimeSec = 0.03;
        const int rampLen = (int) juce::jmax (1.0, sampleRate * smoothTimeSec);

        inGainSmoothed.reset (sampleRate, smoothTimeSec);
        outGainSmoothed.reset (sampleRate, smoothTimeSec);
        thresholdSmoothed.reset (sampleRate, smoothTimeSec);
        symmetrySmoothed.reset (sampleRate, smoothTimeSec);
        biasSmoothed.reset (sampleRate, smoothTimeSec);
        mixSmoothed.reset (sampleRate, smoothTimeSec);

        // Initialize targets to current values so we avoid initial jumps
        inGainSmoothed.setCurrentAndTargetValue (inGain);
        outGainSmoothed.setCurrentAndTargetValue (outGain);
        thresholdSmoothed.setCurrentAndTargetValue (threshold);
        symmetrySmoothed.setCurrentAndTargetValue (symmetry);
        biasSmoothed.setCurrentAndTargetValue (bias);
        mixSmoothed.setCurrentAndTargetValue (mix);

        // DC blocker per channel (disabled by default)
        dcFilters.clear();
        dcFilters.resize ((size_t) numChannels);
        updateDcBlocker();
    }

    void reset()
    {
        inGainSmoothed.reset (sampleRate, 0.0);
        outGainSmoothed.reset (sampleRate, 0.0);
        thresholdSmoothed.reset (sampleRate, 0.0);
        symmetrySmoothed.reset (sampleRate, 0.0);
        biasSmoothed.reset (sampleRate, 0.0);
        mixSmoothed.reset (sampleRate, 0.0);

        for (auto& f : dcFilters)
            f.reset();
    }

    //==========================================================================
    // Setters (RT-safe; they only set target values for smoothed params)
    void setDriveDecibels (float dB)
    {
        driveDb = dB;
        inGain = juce::Decibels::decibelsToGain (juce::jlimit (-60.0f, 60.0f, dB));
        inGainSmoothed.setTargetValue (inGain);
    }

    void setOutputGainDecibels (float dB)
    {
        outDb = dB;
        outGain = juce::Decibels::decibelsToGain (juce::jlimit (-60.0f, 60.0f, dB));
        outGainSmoothed.setTargetValue (outGain);
    }

    /** Folding threshold (amplitude) where reflections begin. Smaller => more folding.
        Typical range: [0.05 .. 1.5]. Default: 0.6
     */
    void setThreshold (float newThreshold)
    {
        threshold = juce::jlimit (1.0e-5f, 10.0f, newThreshold);
        thresholdSmoothed.setTargetValue (threshold);
    }

    /** Asymmetry control in [-1..+1]. Positive values fold earlier on the positive side. */
    void setSymmetry (float newSymmetry)
    {
        symmetry = juce::jlimit (-1.0f, 1.0f, newSymmetry);
        symmetrySmoothed.setTargetValue (symmetry);
    }

    /** Pre-fold bias/offset in linear amplitude units (post-drive, pre-fold). */
    void setBias (float newBias)
    {
        bias = juce::jlimit (-2.0f, 2.0f, newBias);
        biasSmoothed.setTargetValue (bias);
    }

    /** Number of cascaded folding stages (>=1). */
    void setStages (int newStages)
    {
        stages = juce::jlimit (1, 12, newStages);
    }

    /** Dry/Wet mix in [0..1]. 0=dry, 1=wet */
    void setMix (float newMix)
    {
        mix = juce::jlimit (0.0f, 1.0f, newMix);
        mixSmoothed.setTargetValue (mix);
    }

    /** Enable/disable DC blocker (1st-order HPF around 20 Hz). */
    void setDcBlockerEnabled (bool shouldEnable)
    {
        dcBlockerEnabled = shouldEnable;
        updateDcBlocker();
    }

    //==========================================================================
    /** Process in place (replacing) using a JUCE AudioBlock<float>. */
    void process (juce::dsp::AudioBlock<float> block) noexcept
    {
        const auto nCh = (int) juce::jmin<size_t> (numChannels, block.getNumChannels());
        const auto nSmps = (int) block.getNumSamples();

        if (nCh <= 0 || nSmps <= 0)
            return;

        // Per-sample smoothed parameters
        for (int n = 0; n < nSmps; ++n)
        {
            const float gIn   = inGainSmoothed.getNextValue();
            const float gOut  = outGainSmoothed.getNextValue();
            const float thr   = thresholdSmoothed.getNextValue();
            const float sym   = symmetrySmoothed.getNextValue();
            const float b     = biasSmoothed.getNextValue();
            const float wet   = mixSmoothed.getNextValue();

            // Compute asymmetric fold limits from threshold & symmetry
            // sym>0 => smaller positive limit, larger negative limit
            const float symAmt = 0.95f * sym;
            const float posLimit = juce::jmax (1.0e-6f, thr * (1.0f - symAmt));
            const float negLimit = juce::jmax (1.0e-6f, thr * (1.0f + symAmt));

            for (int ch = 0; ch < nCh; ++ch)
            {
                float* s = block.getChannelPointer ((size_t) ch);

                const float dry  = s[n];                    // save dry
                float x = dry * gIn + b;                    // drive + bias

                // Cascade stages to enrich harmonics
                for (int st = 0; st < stages; ++st)
                    x = foldToRange (x, posLimit, -negLimit);

                // Dry/Wet and output gain
                const float y = (1.0f - wet) * dry + wet * x;
                s[n] = y * gOut;
            }
        }
    }

private:
    //==========================================================================
    // Folding by repeated mirror reflections into [lower, upper].
    // Using a simple while-loop (bounded) to handle arbitrary overshoots.
    static inline float foldToRange (float x, float upper, float lower) noexcept
    {
        // Ensure proper ordering (lower should be < upper)
        if (lower > upper)
            std::swap (lower, upper);

        // Hard limits for denormals / edge safety
        upper = juce::jlimit (1.0e-6f, 1000.0f, upper);
        lower = juce::jlimit (-1000.0f, -1.0e-6f, lower);

        // Reflect as many times as needed; bound the loop to avoid pathological cases
        int guard = 0;
        while ((x > upper || x < lower) && guard++ < 32)
        {
            if (x > upper)
                x = upper - (x - upper);  // reflect at +upper
            else
                x = lower - (x - lower);  // reflect at lower (negative)
        }

        // Clip to ensure strict bounds (guard against rounding)
        return juce::jlimit (lower, upper, x);
    }
    
    // Symmetric triangular folding (periodic, smooth)
    static inline float foldTri (float x, float L) noexcept
    {
        if (L <= 1.0e-6f) return 0.0f;
        const float k = juce::MathConstants<float>::pi / (2.0f * L);
        return (2.0f * L / juce::MathConstants<float>::pi) * std::asin (std::sin (k * x));
    }

    //==========================================================================
    // State
    double sampleRate   = 0.0;
    int    maxBlockSize = 0;
    int    numChannels  = 0;

    // Parameters (targets)
    float driveDb   = 0.0f;
    float outDb     = 0.0f;
    float inGain    = 1.0f;   // linear
    float outGain   = 1.0f;   // linear
    float threshold = 0.6f;   // fold limit
    float symmetry  = 0.0f;   // [-1..1]
    float bias      = 0.0f;   // pre-fold offset
    float mix       = 1.0f;   // wet
    int   stages    = 1;

    // Smoothed parameters
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> inGainSmoothed  { 1.0f };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> outGainSmoothed { 1.0f };
    juce::SmoothedValue<float> thresholdSmoothed { 0.6f };
    juce::SmoothedValue<float> symmetrySmoothed  { 0.0f };
    juce::SmoothedValue<float> biasSmoothed      { 0.0f };
    juce::SmoothedValue<float> mixSmoothed       { 1.0f };
};
