#pragma once

#include <juce_dsp/juce_dsp.h>

/**
* @struct ParameterInfo
* @brief A structure to hold the necessary data to create a GUI control.
*/
struct ParameterInfo
{
    enum class Type { Continuous, Boolean };

    juce::String paramID;
    juce::String name;
    float defaultValue;
    float minValue;
    float maxValue;
    juce::String unit = "";
    Type type = Type::Continuous;
};

class IDSPAlgorithm
{
public:
    virtual ~IDSPAlgorithm() = default;

    // --- Core Audio Lifecycle ---
    virtual void prepare (const juce::dsp::ProcessSpec& spec) = 0;
    virtual void processBlock (juce::dsp::AudioBlock<float>& block) = 0;
    virtual void reset() = 0;

    // --- Parameter Management ---
    /**
     * @brief Returns the list of parameters required by this specific algorithm.
     */
    virtual std::vector<ParameterInfo> getParameters() const = 0;

    /**
     * @brief Updates an internal parameter based on a value from the GUI.
     * @param paramID The unique ID of the parameter being updated.
     * @param value The new normalized (0.0 to 1.0) or actual value.
     */
    virtual void updateParameter (const juce::String& paramID, float value) = 0;
};
