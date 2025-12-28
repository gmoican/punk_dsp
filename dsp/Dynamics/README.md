# Dynamics

A set of processors providing various dynamic algorithms for compression and expansion effects.

Jump to each processor:
* [Shared features](#shared-features)
* [Usage examples](#usage-examples)

## Shared features

The `Compressor`, `Lifter` and `Gate` classes share the same logic for applying dynamic processing:
1. Identify sidechain - `Lifter` and `Compressor` can be set to be use _feed-back_ or _feed-forward_ topology.
2. Measure sidechain.
3. Compute gain reduction / addition.
4. Apply gain reduction / addition.

All classes have methods for updating the following parameters:
- Ratio.
- Threshold (in decibels).
- Knee (in decibels).
- Attack and release times (in miliseconds).
- Make-up gain (in decibels).
- Mix (in percentage).
- Feed-back / feed-forward topology (`Lifter` and `Compressor`).

Furthermore, there is a `getGainReduction` (`getGainAddition` in case of the Lifter) method meant to be used in the GUI for displaying the current gain reduction / addition.

## Usage examples

```cpp
// --- PluginProcessor.h ---
#include "punk_dsp/punk_dsp.h"

class PluginProcessor : public juce::AudioProcessor
{
public:
    /* Your processor public stuff
     * ...
     */
private:
    /* Your processor public stuff
     * ...
     */
    punk_dsp::Compressor compressor;
    punk_dsp::Lifter lifter;
    punk_dsp::Gate gate;
};

// --- PluginProcessor.cpp ---
void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    compressor.prepare( spec );
    lifter.prepare( spec );
    gate.prepare( spec );

    // Your code...
}

void PluginProcessor::updateParameters()
{
    // Examples
    compressor.updateThres(-3.f);
    lifter.updateRatio(6.f);
    gate.updateMix(90.f);

    // Your code...
}

void PunkOTTProcessor::processBlock (juce::AudioBuffer<float>& buffer)
{
    gate.process(buffer);
    lifter.process(buffer);
    compressor.process(buffer);
}
```
