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

    void Wavefolder::setBiasPre(float newBiasPre)
    {
        biasPre = juce::jlimit(-1.0f, 1.0f, newBiasPre);
    }

    void Wavefolder::setBiasPost(float newBiasPost)
    {
        biasPost = juce::jlimit(-1.0f, 1.0f, newBiasPost);
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
        auto x = drive * (sample + biasPre) + biasPost;

        // Fold the wave when it exceeds the threshold
        while (std::abs(x) > threshold)
        {
            if (x > threshold)
                x = 2.0f * threshold - x;
            else if (x < -threshold)
                x = -2.0f * threshold - x;
        }

        return outGain * (x * mix + sample * (1.0f - mix));
    }

    float Wavefolder::foldSinSample(float sample)
    {
        auto x = drive * (sample + biasPre) + biasPost;

        // Sine wave folding
        return outGain * (juce::dsp::FastMathApproximations::sin(x) * mix + sample * (1.0f - mix));
    }

    float Wavefolder::extraFoldToRangeSample(float sample)
    {
        while (std::abs(sample) > threshold)
        {
            if (sample > threshold)
                sample = 2.0f * threshold - sample;
            else if (sample < -threshold)
                sample = -2.0f * threshold - sample;
        }
        return sample;
    }

    float Wavefolder::comboFoldSample(float sample)
    {
        // Sine wave folding -> foldToRange folding
        return extraFoldToRangeSample( foldSinSample(sample) );
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

    void Wavefolder::comboFoldBuffer(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = comboFoldSample(channelData[sample]);
        }
    }
}
