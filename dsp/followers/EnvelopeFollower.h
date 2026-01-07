#pragma once

#include "juce_dsp/juce_dsp.h"

namespace punk_dsp
{
    class EnvelopeFollower
    {
    public:
        EnvelopeFollower()
        {
            setAttackTime(10.0f);
            setReleaseTime(100.0f);
        }
        
        void prepare(double sampleRate)
        {
            currentSampleRate = sampleRate;
            updateCoefficients();
            envelope = 0.0f;
        }
        
        void setAttackTime(float attackMs)
        {
            attackTime = attackMs;
            updateCoefficients();
        }
        
        void setReleaseTime(float releaseMs)
        {
            releaseTime = releaseMs;
            updateCoefficients();
        }
        
        float processSample(float input)
        {
            float inputAbs = std::abs(input);
            
            if (inputAbs > envelope)
                envelope += attackCoeff * (inputAbs - envelope);
            else
                envelope += releaseCoeff * (inputAbs - envelope);
            
            return envelope;
        }
        
        // No creo que este método haga falta...
        void processBlock(const juce::AudioBuffer<float>& buffer)
        {
            int numSamples = buffer.getNumSamples();
            int numChannels = buffer.getNumChannels();
            
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float maxSample = 0.0f;
                
                // Find the maximum absolute value across all channels
                for (int channel = 0; channel < numChannels; ++channel)
                {
                    float sampleValue = std::abs(buffer.getSample(channel, sample));
                    maxSample = std::max(maxSample, sampleValue);
                }
                
                processSample(maxSample);
            }
        }
        
        float getCurrentEnvelope() const
        {
            return envelope;
        }
        
        float getCurrentEnvelopeDB() const
        {
            return juce::Decibels::gainToDecibels(envelope, -96.0f);
        }
        
        void reset()
        {
            envelope = 0.0f;
        }

    private:
        // Internal Math Methods
        void updateCoefficients()
        {
            if (currentSampleRate > 0.0)
            {
                attackCoeff = 1.0f - std::exp(-1.0f / (attackTime * 0.001f * currentSampleRate));
                releaseCoeff = 1.0f - std::exp(-1.0f / (releaseTime * 0.001f * currentSampleRate));
            }
        }
        
        float envelope = 0.0f;

        // Parameters
        float attackTime = 10.0f;   // milliseconds
        float releaseTime = 100.0f; // milliseconds

        // Cached values
        float attackCoeff = 0.0f;
        float releaseCoeff = 0.0f;
        double currentSampleRate = 44100.0;

        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
    };
}