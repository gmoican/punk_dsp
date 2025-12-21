# UTILS DOCUMENTATION

This is the documentation for every class in the `utils` folder. If the documentation is not updated, the class you are trying to look for is still under development.

## How to use the `PresetManager` in Your Plugin

1. `PluginProcessor.h`: Add the `PresetManager` as a member.

```C++
#include "MyToolsModule.h" // Includes all your custom headers

class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    // ...
private:
    juce::AudioProcessorValueTreeState apvts;
    MyTools::Utils::PresetManager presetManager { apvts }; // Initialize in the initializer list
    // ...
};
````

2. `PluginProcessor.cpp` (in the constructor):

```C++
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
// ... initialize parameters ...
: apvts (*this, nullptr, "Parameters", createParameterLayout()),
  presetManager (apvts) // Pass the initialized APVTS
{
    // Load the state of the last-used preset immediately after construction
    presetManager.loadLastPreset();
}
````

3. In your GUI (to save a preset):

```C++
// Example call, perhaps from a button click or a text entry confirmation
presetManager.savePreset ("My New Preset");
```

4. In your GUI (to load a preset):

```C++
// Example call, perhaps from a preset menu/combobox selection
presetManager.loadPreset ("Crunchy Lead");
```