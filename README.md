# Punk DSP

I want to write a documentation so clean and curated that even a dumbass like me can understand. If you have any suggestions to make this repository better, do not hesitate to contact me.

## Scope of the project
I intend to write my own classes in this module to better organize my JUCE projects. I expect this module to grow as I attempt to create more plugins.

This is a high-level overview of my current ambitions towards this project:
* **DSP:** My main focus is to write DSP processors. This is where I intend to put most of my work right now. Here are some features that I want to bring in the inmediate future:
    - Parametric waveshapers.
    - Envelope follower.
    - Tube emulation.
* **GUI:** I am a sucker for GUI design and currently prefer to use `Custom Look&Feel` implementations.
* **Utility:** I can only think of a `PresetManager` for this category...

## Installation

### Using CMake
I've gotten used to building my JUCE plugins with CMake so I suggest using this method for adding my module in your project.

1. I like having a separated folder for JUCE modules in my plugin repositories. In this folder, I add this module:

```bash
git submodule add -b main https://github.com/gmoican/punk_dsp.git
```

2. Add to your CMakeLists.txt:
cmake

```cmake
juce_add_module(modules/punk_dsp)

target_link_libraries(YourTarget
    INTERFACE
    my_module
)
```

3. For updating the submodule, the first line should work fine. Sometimes, my module isn't updated in my plugin so I run the second line instead to force the merge.

```bash
git submodule update

git submodule update --remote --merge
```

### Using Projucer

I have not tested if this method works. I suggest sticking to CMake even if it is a pain in the ass to learn... In any case, the method should be the following:

1. Add the module path in Projucer under `File > Add a module...`
2. Select `punk_dsp` from your cloned repository
3. Link it to your project

## Usage

### DSP Modules
For now, all my custom classes are inside the `punk_dsp::` namespace. Since most of my current code is for DSP processors, I don't intend to change this but maybe, in a distant future, I may add a `::dsp:: / ::gui:: / ::utils::` distinction.

All my DSP processors have a very similar approach for implementing them in your `PluginProcessor`. There is a quick guide below, but you can check the `README` in the specific folder of this repo in case of doubt.

```cpp
#include <punk_dsp/punk_dsp.h>

// In your PluginProcessor.h
punk_dsp::Compressor processor;

// PluginProcessor.cpp -> prepare
juce::dsp::ProcessSpec spec;
spec.maximumBlockSize = samplesPerBlock;
spec.numChannels = getTotalNumOutputChannels();
spec.sampleRate = sampleRate;

processor.prepare(spec);

// PluginProcessor.cpp -> Parameter updater function
processor.updateParameter(newParameterValue);

// PluginProcessor.cpp -> process
processor.process(buffer);
```

For detailed documentation on individual classes and processors, see check the `README` files inside the DSP and Utils folders.

## Example projects
Here are some plugins made with **`punk_dsp`**:
* [PunkOTT](https://github.com/gmoican/PunkOTT) (single-band) and [PunkOTT-MB](https://github.com/gmoican/PunkOTT-MB) (multi-band), my personal take of the _Over-The-Top_ style dynamics processor.

## Changelog
### Version 1.0.0
- Initial release
