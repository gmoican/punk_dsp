#pragma once

#include "./IDSPAlgorithm.h"

/**
 * @class Waveshaper
 * @brief Implements several types of waveshaping function
 *
 * Contains some waveshaping functions
 * On its own, it works as a clipper
 * In combination with filters, adequate input gain and bias, it works as the core of distortion processors
 */
class Waveshaper : public IDSPAlgorithm
{
public:
    Waveshaper();
    ~Waveshaper() = default;

    // --- Implementation of the parameter interface methods ---
    std::vector<ParameterInfo> getParameters() const override;
    void updateParameter (const juce::String& paramID, float value) override;

    // Sample processing
    float applySoftClipper(float sample);
    float applyHardClipper(float sample);
    float applyTanhClipper(float sample);
    float applyATanClipper(float sample);
    
    // Buffer processing
    void applySoftClipper(juce::AudioBuffer<float>& processedBuffer);
    void applyHardClipper(juce::AudioBuffer<float>& processedBuffer);
    void applyTanhClipper(juce::AudioBuffer<float>& processedBuffer);
    void applyATanClipper(juce::AudioBuffer<float>& processedBuffer);

private:
    float gainFactor { 1.0f };
    float param { 1.0f };       // Unused for now, will be used in future parametric waveshapers

    // --- Prevent copy and move ---
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Waveshaper)
};
