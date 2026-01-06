#include "Wavefolder.h"

namespace punk_dsp
{
    Wavefolder::Wavefolder()
    {
    }

    // --- --- PARAMETER UPDATES --- --
    void Wavefolder::setDrive(float newDrive)
    {
        drive = newDrive;
    }

    void Wavefolder::setOutGain(float newOutGain)
    {
        outGain = newOutGain;
    }

    void Wavefolder::setBias(float newBias)
    {
        bias = juce::jlimit(-1.0f, 1.0f, newBias);
    }

    void Wavefolder::setSymmetry(float newSymemtry)
    {
        symmetry = juce::jlimit(-1.0f, 1.0f, newSymemtry);
    }

    void Wavefolder::setThreshold(float newThres)
    {
        threshold = juce::jlimit(-1.0f, 1.0f, newThres);
    }

    void Wavefolder::setMix(float newMix)
    {
        mix = juce::jlimit(0.0f, 1.0f, newMix);
    }

    // --- --- SAMPLE PROCESSING --- ---
    float Wavefolder::foldToRangeSample(float sample)
    {
        auto x = drive * sample + bias;

        auto posThres = juce::jlimit(0.05f, threshold * (1.0f - symmetry));
        auto negThres = juce::jlimit(0.05f, threshold * (1.0f - symmetry));

        // Fold the wave when it exceeds the threshold
        while (std::abs(x) > threshold)
        {
            if (x > posThres)
                x = 2.0f * posThres - x;
            else if (x < -negThres)
                x = -2.0f * negThres - x;
        }

        return x * mix + sample * (1.0f - mix);
    }

    float Wavefolder::foldSinSample(float sample)
    {
        auto x = drive * sample + bias;

        // Sine wave folding
        return juce::dsp::FastMathApproximations::sin(x) * mix + sample * (1.0f - mix);
    }

    // --- --- BUFFER PROCESSING --- ---
    void Wavefolder::foldToRangeBuffer(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = foldToRangeSample(channelData[sample]);
        }
    }

    void Wavefolder::foldSinBuffer(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = foldSinSample(channelData[sample]);
        }
    }
}