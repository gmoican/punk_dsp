#pragma once

#include "juce_dsp/juce_dsp.h"
#include "./IDSPAlgorithm.h"

/**
 * @class Compressor
 * @brief Implements a standard, stable, feed-forward downward compressor.
 *
 * All core dynamics processing, including sidechain detection, gain computation,
 * and envelope smoothing, is contained here.
 */
class Compressor : public IDSPAlgorithm
{
public:
    Compressor();
    ~Compressor() = default;

    // --- IDSPAlgorithm Overrides ---
    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void processBlock (juce::dsp::AudioBlock<float>& block) override;
    void reset() override;

    std::vector<ParameterInfo> getParameters() const override;
    void updateParameter (const juce::String& paramID, float value) override;
    
    /*
    void prepare(double sampleRate, int totalNumChannels);
    void updateRatio(float newRatio);
    void updateThres(float newThres);
    void updateKnee(float newKnee);
    void updateAttack(float sampleRate, float newAttMs);
    void updateRelease(float sampleRate, float newRelMs);
    void updateMakeUp(float newMakeUp_dB);
    void updateMix(float newMix);
    */
    
    float getGainReduction();

    /**
     * @brief Processes the audio buffer in-place, applying downward compression.
     *
     * @param inputBuffer The buffer containing the signal to be processed.
     * @param useFeedForward Select when the sidechain signal is measured
     */
    void process(juce::AudioBuffer<float>& inputBuffer);

private:
    // Internal Math Methods (from our previous refactor)
    float calculateTargetGain (float inputDB);
    float updateEnvelope (float targetGR_dB, float currentEnv_dB);
    void updateKneeRange();
    float calculateTimeCoeff (float sampleRate, float time_ms);
    
    // --- Internal State ---
    std::vector<float> envelope;    // Stores the current applied linear gain factor
    float currentGR_dB = 0.0f;      // Gain reduction (in dB) being applied currently

    
    // Parameters
    float ratio          { 4.0f };   // Linear ratio (e.g., 4.0 for 4:1)
    float thresdB        { -12.0f }; // Threshold in dB
    float kneedB         { 6.0f };   // Knee width in dB
    float attackCoeff    { 0.0f };   // Smoothing coefficient (Attack)
    float releaseCoeff   { 0.0f };   // Smoothing coefficient (Release)
    float makeUpGaindB   { 0.0f };   // Compensation gain after the compression takes place
    float mix            { 1.0f };   // Mix (dry/wet)
    float useFeedForward { 1.0f };   // Use feed-forward or feed-back topology
    
    // Cached values for performance
    float sampleRate        { 44100.0f };
    float compressionSlope  { 0.75f }; // 1 - 1/ratio
    float kneeStart         { -15.0f };
    float kneeEnd           { -9.0f };
    float attackCoeff       { 0.0f };
    float releaseCoeff      { 0.0f };

    // --- Prevent copy and move ---
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
};
