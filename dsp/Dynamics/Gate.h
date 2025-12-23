#pragma once

#include "juce_dsp/juce_dsp.h"

/**
 * @class Gate
 * @brief Implements a smooth downward expander (intended to be used as a Noise Gate).
 *
 * All core dynamics processing, including sidechain detection, gain computation,
 * and envelope smoothing, is contained here.
 */
class Gate
{
public:
    Gate();
    ~Gate() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    
    void updateRatio(float newRatio);
    void updateThres(float newThres);
    void updateKnee(float newKnee);
    void updateAttack(float sampleRate, float newAttMs);
    void updateRelease(float sampleRate, float newRelMs);
    void updateMix(float newMix);
    
    float getGainReduction();

    /**
     * @brief Processes the audio buffer in-place, applying a Downward Expander topology.
     *
     * @param inputBuffer The buffer containing the signal to be processed.
     */
    void process(juce::AudioBuffer<float>& inputBuffer);

private:
    // Internal Math Methods
    float calculateTargetGain (float inputDB);
    float updateEnvelope (float targetGR_dB, float currentEnv_dB);
    void updateKneeRange();
    float calculateTimeCoeff (float sampleRate, float time_ms);

    // --- Internal State ---
    ::std::vector<float> envelope; // Stores the current applied linear gain factor
    float currentGR_dB = 0.0f;
    
    // Parameters
    float ratio         = 6.0f;     // Linear ratio
    float thresdB       = -80.0f;   // Threshold in dB
    float kneedB        = 9.0f;     // Knee width in dB
    float attackCoeff   = 0.0f;     // Smoothing coefficient (Attack)
    float releaseCoeff  = 0.0f;     // Smoothing coefficient (Release)
    float mix           = 1.0f;     // Mix (dry/wet)
    
    // Cached values for performance
    float sampleRate        = 44100.0f;
    float expansionSlope    = 5.0f;     // ratio - 1.0f
    float kneeStart         = -100.0f;
    float kneeEnd           = -60.0f;

    // --- Prevent copy and move ---
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Gate)
};
