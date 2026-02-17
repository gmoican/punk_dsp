#include "Compressor.h"

// Define a floor for magnitude detection to prevent log(0) and instability.
static constexpr float compMinMagnitude = 0.00001f;     // Approx -100 dB

namespace punk_dsp
{
    Compressor::Compressor()
    {
    }

    void Compressor::prepare(const juce::dsp::ProcessSpec& spec)
    {
        sampleRate = spec.sampleRate;
        envelope.assign (spec.numChannels, 0.0f);
        
        // Recalculate time coefficients based on current sample rate
        attackCoeff = calculateTimeCoeff (10.0f);
        releaseCoeff = calculateTimeCoeff (100.0f);
    }

    void Compressor::reset()
    {
        std::fill (envelope.begin(), envelope.end(), 0.0f);
    }

    float Compressor::calculateTimeCoeff(float time_ms)
    {
        // return std::exp(-2.0f * juce::MathConstants<float>::pi * 1000.f / time_ms / sampleRate);
        return std::exp(-1.f / (0.001f * time_ms * sampleRate));
    }


    void Compressor::updateRatio(float newRatio)
    {
        ratio = newRatio;
        compressionSlope = 1.0f - (1.0f / newRatio);
    }

    void Compressor::updateThres(float newThres)
    {
        thresdB = newThres;
        updateKneeRange();
    }

    void Compressor::updateKnee(float newKnee)
    {
        kneedB = newKnee;
        updateKneeRange();
    }


    void Compressor::updateKneeRange()
    {
        kneeStart = thresdB - (kneedB / 2.0f);
        kneeEnd = thresdB + (kneedB / 2.0f);
    }

    void Compressor::updateAttack(float newAttMs)
    {
        attackCoeff = calculateTimeCoeff(newAttMs);
    }

    void Compressor::updateRelease(float newRelMs)
    {
        releaseCoeff = calculateTimeCoeff(newRelMs);
    }

    void Compressor::updateMakeUp(float newMakeUp_dB)
    {
        makeUpGaindB = newMakeUp_dB;
    }

    void Compressor::updateMix(float newMix)
    {
        mix = newMix / 100.0f;
    }

    void Compressor::updateFeedForward(bool newFeedForward)
    {
        useFeedForward = newFeedForward;
    }

    // --- Core Math Logic ---

    float Compressor::calculateTargetGain(float inputDB)
    {
        if (inputDB > kneeEnd)
            return (inputDB - thresdB) * compressionSlope;
        
        if (inputDB > kneeStart)
        {
            const float x = inputDB - kneeStart;
            return (compressionSlope / (2.0f * kneedB)) * (x * x);
        }

        return 0.0f;
    }

    float Compressor::updateEnvelope(float targetGR_dB, float currentEnv_dB)
    {
        // If target reduction is greater than current (Attack), or less (Release)
        float alpha = (targetGR_dB > -currentEnv_dB) ? attackCoeff : releaseCoeff;
        
        // Exponential smoothing: y[n] = a * y[n-1] + (1-a) * x[n]
        float smoothedReduction = (alpha * -currentEnv_dB) + ((1.0f - alpha) * targetGR_dB);
        return -smoothedReduction;
    }

    // --- Methods for GUI ---
    float Compressor::getGainReduction()
    {
        return currentGR_dB;
    }

    void Compressor::process(juce::AudioBuffer<float>& inputBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();
        
        if ((int)envelope.size() != numChannels)
            envelope.assign(numChannels, 0.0f);
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* channelData = inputBuffer.getWritePointer(ch);
            float currentEnv = envelope[ch];
            
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float input = channelData[sample];
                
                // 1. Identify Sidechain Input based on Topology
                float sidechainInput = input;
                
                if (!useFeedForward)
                {
                    // Feed-back uses the PREVIOUS output (estimated by current envelope)
                    float prevGain = juce::Decibels::decibelsToGain(currentEnv + makeUpGaindB);
                    sidechainInput = input * prevGain;
                }
                
                // 2. Detection (Sidechain)
                float mag = std::max( std::abs(sidechainInput), compMinMagnitude );
                float inputDB = juce::Decibels::gainToDecibels(mag);
                
                // 3. Gain Computer & Ballistics
                float targetGR = calculateTargetGain(inputDB);
                currentEnv = updateEnvelope(targetGR, currentEnv);
                
                // 4. Apply Gain
                float totalGainDB = currentEnv + makeUpGaindB;
                float gainLinear = juce::Decibels::decibelsToGain(totalGainDB);
                
                float processed = input * gainLinear;
                channelData[sample] = (processed * mix) + (input * (1.0f - mix));
            }
            
            envelope[ch] = currentEnv;
            
            if (ch == 0) currentGR_dB = currentEnv; // For meter reporting
        }
    }

    void Compressor::processWithSidechaing(juce::AudioBuffer<float>& inputBuffer, juce::AudioBuffer<float>& sidechainBuffer)
    {
        const int numSamples = inputBuffer.getNumSamples();
        const int numChannels = inputBuffer.getNumChannels();
        
        if ((int)envelope.size() != numChannels)
            envelope.assign(numChannels, 0.0f);
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* sidechainData = sidechainBuffer.getReadPointer(ch);
            float* channelData = inputBuffer.getWritePointer(ch);
            float currentEnv = envelope[ch];
            
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float input = channelData[sample];
                
                // 1. Identify Sidechain Input based on Topology
                float sidechainInput = sidechainData[sample];
                
                if (!useFeedForward)
                {
                    // Feed-back uses the PREVIOUS output (estimated by current envelope)
                    float prevGain = juce::Decibels::decibelsToGain(currentEnv + makeUpGaindB);
                    sidechainInput = sidechainInput * prevGain;
                }
                
                // 2. Detection (Sidechain)
                float mag = std::max( std::abs(sidechainInput), compMinMagnitude );
                float inputDB = juce::Decibels::gainToDecibels(mag);
                
                // 3. Gain Computer & Ballistics
                float targetGR = calculateTargetGain(inputDB);
                currentEnv = updateEnvelope(targetGR, currentEnv);
                
                // 4. Apply Gain
                float totalGainDB = currentEnv + makeUpGaindB;
                float gainLinear = juce::Decibels::decibelsToGain(totalGainDB);
                
                float processed = input * gainLinear;
                channelData[sample] = (processed * mix) + (input * (1.0f - mix));
            }
            
            envelope[ch] = currentEnv;
            
            if (ch == 0) currentGR_dB = currentEnv; // For meter reporting
        }
    }
}
