#include "Waveshaper.h"

namespace punk_dsp
{
    Waveshaper::Waveshaper()
    {
    }

    // --- --- PARAMETER UPDATES --- --
    void Waveshaper::setInGain(float newInGain)
    {
        inGain = newInGain;
    }

    void Waveshaper::setOutGain(float newOutGain)
    {
        outGain = newOutGain;
    }

    void Waveshaper::setBiasPre(float newBiasPre)
    {
        biasPre = juce::jlimit(-1.0f, 1.0f, newBiasPre);
    }

    void Waveshaper::setBiasPost(float newBiasPost)
    {
        biasPost = juce::jlimit(-1.0f, 1.0f, newBiasPost);
    }

    // --- --- SAMPLE PROCESSING --- ---
    float Waveshaper::applySoftClipper(float sample)
    {
        sample = (inGain + biasPre) * sample + biasPost;
        return outGain * sample / (std::abs(sample) + 1.0f);
    }

    float Waveshaper::applyHardClipper(float sample)
    {
        sample = (inGain + biasPre) * sample + biasPost;
        if (sample > 1.0f)
            return outGain * 2.0f / 3.0f;
        else if (sample < -1.0f)
            return outGain * -2.0f / 3.0f;
        else
            return outGain * (sample - std::pow(sample, 3.0f) / 3.0f);
    }

    float Waveshaper::applyTanhClipper(float sample)
    {
        sample = (inGain + biasPre) * sample + biasPost;
        return outGain * 2.0f / juce::MathConstants<float>::pi * juce::dsp::FastMathApproximations::tanh(sample);
    }

    float Waveshaper::applyATanClipper(float sample)
    {
        sample = (inGain + biasPre) * sample + biasPost;
        return outGain * 2.0f / juce::MathConstants<float>::pi * std::atan(sample);
    }

    // --- --- BUFFER PROCESSING --- ---
    void Waveshaper::applySoftClipper(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = applySoftClipper(channelData[sample]);
        }
    }

    void Waveshaper::applyHardClipper(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = applyHardClipper(channelData[sample]);
        }
    }

    void Waveshaper::applyTanhClipper(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = applyTanhClipper(channelData[sample]);
        }
    }

    void Waveshaper::applyATanClipper(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = applyATanClipper(channelData[sample]);
        }
    }
}