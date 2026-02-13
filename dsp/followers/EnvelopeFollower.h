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

        void setAttack(float newAttMs);
        void setRelease(float newRelMs);
        void setPeakDetection(bool newUsePeakDetector);
        
        float getEnvelope();
        float updateAndReturnEnvelope(float input_lin);

    private:
        float calculateTimeCoeff (float time_ms);
        void updateEnvelope (float input_lin);
        float peakDetection(float input_lin);
        float rmsDetection(float input_lin);

        // --- Envelope State ---
        float envelope_lin { 0.0f };    // Current envelope value
                                        // Should be confined between 0 and 1
        float meanSquare   { 0.0f };    // For RMS calculation
        
        // Parameters
        float attackCoeff  { 0.0f };   // Smoothing coefficient (Attack)
        float releaseCoeff { 0.0f };   // Smoothing coefficient (Release)
        float attackTime   { 1.0f };   // Attack time in ms
        float releaseTime  { 1.0f };   // Release time in ms
        
        bool usePeakDetector = true;   // TRUE = Use peak detection
                                       // FALSE = Use RMS detection
        
        // Cached values for performance
        float sampleRate { 44100.0f };
        
        // --- Prevent copy and move ---
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeFollower)
    };
}
