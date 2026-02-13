#include "Lifter.h"

// Define a small constant to prevent division by zero near the noise floor.
static constexpr float lifterMinMagnitude = 0.0001f; // Approx -100 dB

namespace punk_dsp
{
    Lifter::Lifter()
    {
        // Initialize envelope vector size to 0
    }

    void Lifter::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        envelope.assign (spec.numChannels, 1.0f);
        
        // Recalculate time coefficients based on current sample rate
        attackCoeff = calculateTimeCoeff (10.0f);
        releaseCoeff = calculateTimeCoeff (100.0f); 
    }

    void Lifter::reset()
    {
        std::fill (envelope.begin(), envelope.end(), 1.0f);
    }

    float Lifter::calculateTimeCoeff(float time_ms)
    {
        return std::exp(-2.0f * juce::MathConstants<float>::pi * 1000.f / time_ms / sampleRate);
    }

    void Lifter::updateRatio(float newRatio)
    {
        ratio = newRatio;
        compressionSlope = 1.0f - (1.0f / ratio);
    }

    void Lifter::updateRange(float newRange)
    {
        rangedB = newRange;
    }

    void Lifter::updateKnee(float newKnee)
    {
        kneedB = newKnee;
    }

    void Lifter::updateAttack(float newAttMs)
    {
        attackCoeff = calculateTimeCoeff(newAttMs);
    }

    void Lifter::updateRelease(float newRelMs)
    {
        releaseCoeff = calculateTimeCoeff(newRelMs);
    }

    void Lifter::updateMakeUp(float newMakeUp_dB)
    {
        makeUpGain_linear = juce::Decibels::decibelsToGain(newMakeUp_dB);
    }

    void Lifter::updateMix(float newMix)
    {
        mix = newMix / 100.0f;
    }

    void Lifter::updateFeedForward(bool newFeedForward)
    {
        useFeedForward = newFeedForward;
    }

    // --- Core Math Logic ---

    float Lifter::calculateTargetGain(float inputDB)
    {
        float targetGR = 0.0f;
        if (inputDB < kneeStart)
        {
            targetGR = (rangedB - inputDB) * compressionSlope;
        }
        else if (inputDB < kneeEnd)
        {
            const float x = kneeEnd - inputDB;
            targetGR = compressionSlope / (2.0f * kneedB) * (x * x);
        }
        
        targetGR = juce::Decibels::decibelsToGain(targetGR);
        
        return targetGR;
    }

    float Lifter::updateEnvelope(float targetGR_lin, float currentEnv_lin)
    {
        float alpha = (targetGR_lin > currentEnv_lin) ? attackCoeff : releaseCoeff;
        
        float gainAddition = (alpha * currentEnv_lin) + ((1.0f - alpha) * targetGR_lin);
        return juce::jlimit(0.0f, 100.0f, gainAddition);
    }

    // --- Methods for GUI ---
    float Lifter::getGainAddition()
    {
        return juce::Decibels::gainToDecibels(currentGA_linear);
    }

    void Lifter::process(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();
        
        // Ensure envelope vector is correctly sized (safety)
        if ((int)envelope.size() != numChannels)
            envelope.assign(numChannels, 1.0f);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            // Pointers for reading input and writing wet output
            float* channelData = inputBuffer.getWritePointer(channel);
            currentGA_linear = envelope[channel];

            for (int sample = 0; sample < numSamples; ++sample)
            {
                float inputSample = channelData[sample];
                
                // 1. Identify Sidechain Input based on Topology
                float sidechainInput = inputSample;
                
                if (!useFeedForward)
                {
                    // Feed-back uses the PREVIOUS output (estimated by current envelope)
                    float prevGain = currentGA_linear * makeUpGain_linear;
                    sidechainInput = inputSample * prevGain;
                }

                // 2. Detection (Sidechain)
                const float magnitude = std::max( std::abs(sidechainInput), lifterMinMagnitude );
                const float inputDB = juce::Decibels::gainToDecibels(magnitude);
                
                // 3. ENVELOPE SMOOTHING (in dB)
                float targetGR_lin = calculateTargetGain(inputDB);
                currentGA_linear = updateEnvelope(targetGR_lin, currentGA_linear);
                
                // 4. APPLY GAIN (in-place)
                channelData[sample] = inputSample * currentGA_linear * makeUpGain_linear * mix + inputSample * (1.0f - mix);
            }

            // Store the final envelope value for the start of the next block
            envelope[channel] = currentGA_linear;
        }
    }
}
