#include "Waveshaper.h"

Waveshaper::Waveshaper()
{
}

// --- --- PARAMS INTERFACE --- ---
std::vector<ParameterInfo> Waveshaper::getParameters() const
{
    return
    {
        { "gain", "Gain", 1.0f, 0.0f, 20.0f, "" },
        { "param", "Param", 1.0f, -0.9f, 20.0f, "" }
    };
}

void Waveshaper::updateParameter (const juce::String& paramID, float value)
{
    if (paramID == "gain")
        gainFactor = value;
    else if (paramID == "param")
        param = value;
    // else ignore unknown parameter 
}

// --- --- PARAMETER UPDATES --- --
void Waveshaper::setGainFactor(float newGain)
{
    gainFactor = newGain;
}

void Waveshaper::setParamFactor(float newParam)
{
    param = newParam;
}

// --- --- SAMPLE PROCESSING --- ---
float Waveshaper::applySoftClipper(float sample)
{
    return gainFactor * sample / (std::abs(sample) + 1.0f);
}

float Waveshaper::applyHardClipper(float sample)
{
    if (sample > 1.0f)
        return gainFactor * 2.0f / 3.0f;
    else if (sample < -1.0f)
        return gainFactor * -2.0f / 3.0f;
    else
        return gainFactor * (sample - std::pow(sample, 3.0f) / 3.0f);
}

float Waveshaper::applyTanhClipper(float sample)
{
    return gainFactor * 2.0f / juce::MathConstants<float>::pi * juce::dsp::FastMathApproximations::tanh(sample);
}

float Waveshaper::applyATanClipper(float sample)
{
    return gainFactor * 2.0f / juce::MathConstants<float>::pi * std::atan(sample);
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
            channelData[sample] = applyTanhClipper(channelData(sample));
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
            channelData[sample] = applyATanClipper(channelData(sample));
    }
}
