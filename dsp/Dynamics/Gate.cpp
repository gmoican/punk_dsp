#include "Gate.h"

// Define a small constant to prevent division by zero near the noise floor.
static constexpr float gateMinMagnitude = 0.000001f; // Approx -120 dB

namespace punk_dsp
{
    Gate::Gate()
    {
    }

    void Gate::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        envelope.assign (spec.numChannels, 0.0f);
        
        // Recalculate time coefficients based on current sample rate
        attackCoeff = calculateTimeCoeff (10.0f);
        releaseCoeff = calculateTimeCoeff (10.0f);
    }

    void Gate::reset()
    {
        std::fill (envelope.begin(), envelope.end(), 0.0f);
    }

    float Gate::calculateTimeCoeff(float time_ms)
    {
        // return std::exp(-2.0f * juce::MathConstants<float>::pi * 1000.f / time_ms / sampleRate);
        return std::exp(-1.f / (0.001f * time_ms * sampleRate));
    }

    void Gate::updateRatio(float newRatio)
    {
        ratio = newRatio;
        expansionSlope = ratio - 1.0f;
    }

    void Gate::updateThres(float newThres)
    {
        thresdB = newThres;
        updateKneeRange();
    }

    void Gate::updateKnee(float newKnee)
    {
        kneedB = newKnee;
        updateKneeRange();
    }

    void Gate::updateKneeRange()
    {
        kneeStart = thresdB - (kneedB / 2.0f);
        kneeEnd = thresdB + (kneedB / 2.0f);
    }

    void Gate::updateAttack(float newAttMs)
    {
        attackCoeff = calculateTimeCoeff(newAttMs);
    }

    void Gate::updateRelease(float newRelMs)
    {
        releaseCoeff = calculateTimeCoeff(newRelMs);
    }

    void Gate::updateMix(float newMix)
    {
        mix = newMix / 100.0f;
    }

    // --- Core Math Logic ---

    float Gate::calculateTargetGain(float input_dB)
    {
        if (input_dB < kneeStart)
            return (input_dB - thresdB) * expansionSlope;
        if (input_dB < kneeEnd)
        {
            const float x = input_dB - kneeEnd;
            return expansionSlope / (2.0f * kneedB) * (x * x);
        }
        
        return 0.0f;
    }

    float Gate::updateEnvelope(float targetGR_dB, float currentEnv_dB)
    {
        // If target reduction is greater than current (Attack), or less (Release)
        float alpha = (-targetGR_dB > -currentGR_dB) ? attackCoeff : releaseCoeff;
        
        // Exponential smoothing: y[n] = a * y[n-1] + (1-a) * x[n]
        float smoothedReductionAmount = (alpha * -currentGR_dB) + ((1.0f - alpha) * -targetGR_dB);
        return -smoothedReductionAmount;
    }

    // --- Methods for GUI ---
    float Gate::getGainReduction()
    {
        return currentGR_dB;
    }

    // --- PROCESS ---
    void Gate::process(juce::AudioBuffer<float>& inputBuffer)
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
            currentGR_dB = envelope[channel];

            for (int sample = 0; sample < numSamples; ++sample)
            {
                float inputSample = channelData[sample];
                float magnitude = std::max( std::abs (inputSample), gateMinMagnitude );
                
                // 1. SIDECHAIN: Convert magnitude to dB
                const float inputDB = juce::Decibels::gainToDecibels(magnitude);

                // 2. Gain Computer & Ballistics
                float targetGR_dB = calculateTargetGain(inputDB);
                currentGR_dB = updateEnvelope(targetGR_dB, currentGR_dB);
                
                // 4. APPLY GAIN (in-place)
                const float gainReductionLinear = juce::Decibels::decibelsToGain(currentGR_dB);
                float processed = inputSample * gainReductionLinear;
                channelData[sample] = (processed * mix) + (inputSample * (1.0f - mix));
            }
            
            // Store the final envelope value for the start of the next block
            envelope[channel] = currentGR_dB;        
        }
    }

    void Gate::processWithSidechain(juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& sidechainBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();
        
        // Ensure envelope vector is correctly sized (safety)
        if ((int)envelope.size() != numChannels)
            envelope.assign(numChannels, 1.0f);

        for (int channel = 0; channel < numChannels; ++channel)
        {
            // Pointers for reading input and writing wet output
            float* sidechainData = sidechainBuffer.getReadPointer(ch);
            float* channelData = inputBuffer.getWritePointer(channel);
            currentGR_dB = envelope[channel];

            for (int sample = 0; sample < numSamples; ++sample)
            {
                float inputSample = channelData[sample];

                // 1. SIDECHAIN
                float sidechainInput = sidechainData[sample];
                float magnitude = std::max( std::abs (sidechainInput), gateMinMagnitude );
                
                // Convert magnitude to dB
                const float inputDB = juce::Decibels::gainToDecibels(magnitude);

                // 2. Gain Computer & Ballistics
                float targetGR_dB = calculateTargetGain(inputDB);
                currentGR_dB = updateEnvelope(targetGR_dB, currentGR_dB);
                
                // 4. APPLY GAIN (in-place)
                const float gainReductionLinear = juce::Decibels::decibelsToGain(currentGR_dB);
                float processed = inputSample * gainReductionLinear;
                channelData[sample] = (processed * mix) + (inputSample * (1.0f - mix));
            }
            
            // Store the final envelope value for the start of the next block
            envelope[channel] = currentGR_dB;        
        }
    }
}
