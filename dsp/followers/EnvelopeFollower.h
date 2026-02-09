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

        void setAttackTime(float newAttMs);
        void setReleaseTime(float newRelMs);
        
        float getEnvelope();
        float updateAndReturnEnvelope(float input_lin);

    private:
        float calculateTimeCoeff (float time_ms);
        void updateEnvelope (float input_lin);

        // --- Internal State ---
        float envelope_lin { 0.0f };    // Current envelope value
                                        // Should be confined between 0 and 1
        
        // Parameters
        float attackCoeff  { 0.0f };   // Smoothing coefficient (Attack)
        float releaseCoeff { 0.0f };   // Smoothing coefficient (Release)
        
        // Cached values for performance
        float sampleRate { 44100.0f };
        
        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeFollower)
    };
}
