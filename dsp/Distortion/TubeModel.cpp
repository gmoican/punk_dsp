#include "TubeModel.h"

namespace punk_dsp
{
    TubeModel::TubeModel()
    {
    }

    // --- --- PARAMETER UPDATES --- --
    void TubeModel::setDrive(float newDrive)
    {
        drive = newDrive;
    }

    void TubeModel::setOutGain(float newOutGain)
    {
        outGain = newOutGain;
    }

    void TubeModel::setBiasPre(float newBiasPre)
    {
        biasPre = juce::jlimit(-1.0f, 1.0f, newBiasPre);
    }

    void TubeModel::setBiasPost(float newBiasPost)
    {
        biasPost = juce::jlimit(-1.0f, 1.0f, newBiasPost);
    }

    void TubeModel::setCoeffPos(float newCoeffPos)
    {
        coeffPos = juce::jlimit(0.0f, 2.0f, newCoeffPos);
    }

    void TubeModel::setCoeffNeg(float newCoeffNeg)
    {
        coeffNeg = juce::jlimit(0.0f, 2.0f, newCoeffNeg);
    }

    void TubeModel::setHarmonicGain(float newHarmGain)
    {
        harmonicGain = newHarmGain;
    }
    
    void TubeModel::setHarmonicBalance(float newBalance)
    {
        harmonicBalance = juce::jlimit(0.0f, 1.0f, newBalance);
    }

    void TubeModel::setHarmonicSidechain(bool usePostDrive)
    {
        harmonicSidechain = usePostDrive;
    }

    void TubeModel::setSagTime(float time_ms)
    {
        sagTime = juce::jlimit(0.1f, 100.0f, time_ms);
    }

    // --- --- PROCESSING --- ---
    float TubeModel::processSample(float sample)
    {
        float x = (sample + biasPre) * drive + biasPost;
        float output = 0.0f;

        // Asymmetric transfer function
        if (x > 0.0f)
            output = x / (1.0f + coeffPos * std::abs(x));
        else
            output = x / (1.0f + coeffNeg * std::abs(x));

        // Generate harmonics
        float harmonics = 0.0f;
        if (harmonicSidechain)
            harmonics = harmonicGain * addHarmonics(output);
        else
            harmonics = harmonicGain * addHarmonics(x);

        calculateSag(x);

        return (output + harmonics) * sagResponse * outGain;
    }

    void TubeModel::processBuffer(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float* channelData = inputBuffer.getWritePointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
                channelData[sample] = processSample(channelData[sample]);
        }
    }

    // --- --- EXTRA STEPS --- ---
    float TubeModel::addHarmonics(float inputSignal)
    {
        return harmonicBalance * juce::dsp::FastMathApproximations::sin(2.0f * juce::MathConstants<float>::pi * inputSignal) + (1.0f - harmonicBalance) * juce::dsp::FastMathApproximations::sin(3.0f * juce::MathConstants<float>::pi * inputSignal);
    }

    void TubeModel::calculateSag(float inputSignal)
    {
        float signalMagnitude = std::abs(x);
        float decayFactor = std::exp(-0.001f * signalMagnitude * (1000.0f / sagTime));
        sagResponse = std::max(0.1f, sagResponse * decayFactor + 0.01f * signalMagnitude);
    }
}