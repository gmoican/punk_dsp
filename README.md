# Punk DSP

**WARNING**: This module is a work in progress. As long as you see this text here, you should not trust anything that goes below.

I want to write a documentation so clean and curated that even a dumbass like me can understand. I hope you understand how hard that can be...

**Inmediate to-dos:**
- Implement workbench plugin
- Test waveshapers
- Refactor everything inside Dynamics
    - Write Envelope-follower ??

---------

A collection of utility classes for DSP, GUI, and general utilities I am using across multiple JUCE projects.

## Features

- **DSP Utilities**: Description of your DSP classes
- **GUI Components**: Description of your GUI components
- **General Utilities**: Description of your utility classes

## Requirements

- JUCE 7.0.0 or later
- C++17 or later
- CMake 3.16 or later (for CMake integration)

## Installation

### Using CMake

1. Clone or add this module to your project:

```bash
git clone https://github.com/yourusername/my_module.git
```

2. Add to your CMakeLists.txt:
cmake

```cmake
juce_add_module(/path/to/my_module)

target_link_libraries(YourTarget
    PRIVATE
        my_module
)
```

### Using Projucer

1. Add the module path in Projucer under `File > Add a module...`
2. Select `punk_dsp` from your cloned repository
3. Link it to your project

## Usage

### DSP Modules

```cpp
#include <punk_dsp/punk_dsp.h>

// In your PluginProcessor.h
punk_dsp::Compressor processor;

// PluginProcessor.cpp -> prepare
processor.prepare(sampleRate, blockSize);

// PluginProcessor.cpp -> prepare
processor.process(buffer);
```

### GUI Modules

```cpp
#include <punk_dsp/punk_dsp.h>

// In your PluginEditor.h
punk_dsp::LevelMeter levelMeter;

// PluginEditor.cpp
addAndMakeVisible(levelMeter);
```

## Documentation

For detailed documentation on individual classes, see the inline comments in the header files.

## License
This module is licensed under the MIT license. See LICENSE file for details.

## Contributing

Contributions are welcome! Please ensure:
- Code follows the existing style
- All changes are tested
- Documentation is updated

## Author
Guillermo Moñino Cánovas - your.email@example.com

## Changelog
### Version 1.0.0
- Initial release
