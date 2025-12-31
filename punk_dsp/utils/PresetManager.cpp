#include "PresetManager.h"

// Set the directory where presets will be stored relative to the application's
// data path.
static const juce::String presetSubFolder = "MyToolsPresets";
static const juce::String presetExtension = ".xml";

//==========================================================================
PresetManager::PresetManager (juce::AudioProcessorValueTreeState& vts)
    : apvts (vts)
{
    // 1. Get a safe location to store application data (usually 
    //    ~/Library/Application Support on macOS or C:\Users\...\AppData on Windows)
    juce::File appDataDir = juce::File::getSpecialLocation (
        juce::File::SpecialLocationType::userApplicationDataDirectory
    ).getChildFile (JucePlugin_Name); // Assuming JucePlugin_Name is defined in your module.h

    // 2. Set the specific preset directory path
    presetDirectory = appDataDir.getChildFile (presetSubFolder);

    /* NOT WORKING - TODO: FIX
    // 3. Ensure the directory exists. Create it if it doesn't.
    if (presetDirectory.createDirectory().wasSuccessful())
        DBG ("Preset directory created: " + presetDirectory.getFullPathName());
    else
        DBG ("Error creating preset directory: " + presetDirectory.getFullPathName());
    
    // 4. Initialize the 'lastPreset' property in the APVTS if it doesn't exist
    // This ensures the property is ready to store the last used preset name
    if (apvts.state.getProperty(lastPresetProperty).toString().isEmpty())
    {
            apvts.state.setProperty(lastPresetProperty, currentPresetName, nullptr);
    }
    */
}

//==========================================================================
juce::StringArray PresetManager::getPresetNames() const
{
    juce::StringArray names;
    /* NOT WORKING - TODO: FIX
    // Find all files in the directory with the correct extension
    for (const auto& entry : presetDirectory.findChildFiles (
        juce::File::FindFlags::findFiles, 0, "*" + presetExtension))
    {
        // Remove the extension to get the clean preset name
        names.add (entry.getFileNameWithoutExtension());
    }
    */
    return names;
}

//==========================================================================
juce::File PresetManager::getFileForPreset (const juce::String& presetName) const
{
    return presetDirectory.getChildFile (presetName + presetExtension);
}

//==========================================================================
void PresetManager::savePreset (const juce::String& presetName)
{
    // 1. Get the current state of the plugin as an XML object
    ::std::unique_ptr<juce::XmlElement> xml (apvts.copyState().createXml());
    
    // 2. Write the XML data to the designated file path
    if (xml != nullptr)
    {
        auto presetFile = getFileForPreset (presetName);
        if (xml->writeTo (presetFile))
        {
            currentPresetName = presetName;
            apvts.state.setProperty (lastPresetProperty, presetName, nullptr);
            DBG ("Preset saved: " + presetName);
        }
        else
        {
            DBG ("ERROR: Failed to write preset file: " + presetFile.getFullPathName());
        }
    }
}

//==========================================================================
void PresetManager::loadPreset (const juce::String& presetName)
{
    auto presetFile = getFileForPreset (presetName);

    if (! presetFile.existsAsFile())
    {
        DBG ("ERROR: Preset file not found: " + presetName);
        return;
    }

    // 1. Read the XML data from the file
    ::std::unique_ptr<juce::XmlElement> xml (juce::XmlDocument::parse (presetFile));
    
    if (xml != nullptr && xml->hasTagName (apvts.state.getType()))
    {
        // 2. Create a ValueTree from the XML
        juce::ValueTree tree = juce::ValueTree::fromXml (*xml);
        /* NOT WORKING - TODO: FIX
        // 3. Attempt to load the state into the APVTS
        if (apvts.copyStateIfDifferent (tree))
        {
            currentPresetName = presetName;
            apvts.state.setProperty (lastPresetProperty, presetName, nullptr);
            DBG ("Preset loaded: " + presetName);
        }
        else
        {
            DBG ("ERROR: Failed to load state from XML for preset: " + presetName);
        }
        */
    }
}

//==========================================================================
void PresetManager::loadLastPreset()
{
    juce::String lastPreset = apvts.state.getProperty (lastPresetProperty).toString();
    
    if (! lastPreset.isEmpty() && lastPreset != "Default")
    {
        loadPreset (lastPreset);
    }
    else
    {
        // Set the current name to "Default" if no last preset was found
        currentPresetName = "Default";
    }
}
