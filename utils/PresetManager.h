#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h> // For file browsing

/**
    * @class PresetManager
    * @brief Handles the loading, saving, and management of plugin presets 
    * using the AudioProcessorValueTreeState.
    */
class PresetManager
{
public:
    //======================================================================
    /**
    * @brief Constructor.
    * @param vts A reference to the plugin's AudioProcessorValueTreeState.
    */
    PresetManager (juce::AudioProcessorValueTreeState& vts);

    /**
    * @brief Returns a list of all currently saved preset names.
    * @return A StringArray containing the names of all presets.
    */
    juce::StringArray getPresetNames() const;

    /**
    * @brief Saves the current plugin state as a preset.
    * * @param presetName The name to save the preset as.
    */
    void savePreset (const juce::String& presetName);

    /**
    * @brief Loads a preset by name and applies the settings to the APVTS.
    * * @param presetName The name of the preset to load.
    */
    void loadPreset (const juce::String& presetName);

    /**
    * @brief Loads the last saved/used preset on initialization.
    */
    void loadLastPreset();
    
    /**
    * @brief Returns the name of the currently active preset.
    */
    juce::String getCurrentPresetName() const { return currentPresetName; }

private:
    //======================================================================
    
    juce::AudioProcessorValueTreeState& apvts;
    juce::File presetDirectory;
    juce::String currentPresetName { "Default" };
    
    // Key used to store the name of the last loaded preset in the APVTS itself
    static constexpr const char* lastPresetProperty = "lastPreset";

    /**
    * @brief Gets the file path for a given preset name.
    */
    juce::File getFileForPreset (const juce::String& presetName) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetManager)
};
