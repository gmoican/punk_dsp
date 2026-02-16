#pragma once

#include "juce_dsp/juce_dsp.h"

namespace punk_dsp
{
    class EnvelopeFollower
    {
    public:
        EnvelopeFollower();
        ~EnvelopeFollower() = default;

        void prepare(const juce::dsp::ProcessSpec& spec);
        void reset();

        void setSampleRate(float newSampleRate);
        void setAttack(float newAttMs);
        void setRelease(float newRelMs);
        
        float getEnvelope();
        float process(float input_lin);

    private:
        float calculateTimeCoeff (float time_ms);

        // --- Envelope State ---
        float envelope { 0.0f };    // Current envelope value
                                    // Should be confined between 0 and 1
        
        // Parameters
        float attackCoeff  { 0.0f };   // Smoothing coefficient (Attack)
        float releaseCoeff { 0.0f };   // Smoothing coefficient (Release)
        float attackTime   { 1.0f };   // Attack time in ms
        float releaseTime  { 1.0f };   // Release time in ms
        
        // Cached values for performance
        float sampleRate { 44100.0f };
        
        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeFollower)
    };
}
