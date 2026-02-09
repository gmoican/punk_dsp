#include "ParametricWaveshaper.h"

namespace punk_dsp
{
    ParametricWaveshaper::ParametricWaveshaper()
    {
    }

    void ParametricWaveshaper::setDrive_dB(float newDrive_dB)
    {
        drive = juce::Decibels::decibelsToGain(newDrive_dB);
    }

    void ParametricWaveshaper::setDrive_lin(float newDrive_lin)
    {
        drive = newDrive_lin;
    }

    void ParametricWaveshaper::setOutGain_dB(float newOutGain_dB)
    {
        outGain = juce::Decibels::decibelsToGain(newOutGain_dB);
    }


    void ParametricWaveshaper::setOutGain_lin(float newOutGain_lin)
    {
        outGain = newOutGain_lin;
    }

    void ParametricWaveshaper::setParam(float newParam)
    {
        param = juce::jlimit(-1.0f, 1.0f, newParam);
    }

    void ParametricWaveshaper::setBiasPre(float newBiasPre)
    {
        biasPre = juce::jlimit(-1.0f, 1.0f, newBiasPre);
    }

    void ParametricWaveshaper::setBiasPost(float newBiasPost)
    {
        biasPost = juce::jlimit(-1.0f, 1.0f, newBiasPost);
    }

    void ParametricWaveshaper::setMix(float newMix)
    {
        mix = newMix / 100.0f;
    }

    float ParametricWaveshaper::processSample(float sample)
    {
        float x = (sample + biasPre) * drive + biasPost;
        float y = (x * (std::abs(x) + param) / (x * x + (param - 1) * std::abs(x) + 1)) * outGain;
        return y * mix + sample * (1.f - mix);
    }

    void ParametricWaveshaper::processBuffer(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = inputBuffer.getWritePointer(ch);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = processSample(channelData[sample]);
        }
    }
}
