#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "punk_dsp/punk_dsp.h"

class WorkbenchProcessor : public juce::AudioProcessor
{
public:
    WorkbenchProcessor();
    ~WorkbenchProcessor() override;

    // DEFAULT STUFF ===============================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // --- Workbench Logic ---
    enum class AlgorithmType { None, Compressor, Lifter, Gate, Waveshaper };
    void setAlgorithm(AlgorithmType type);
    AlgorithmType getCurrentType() const { return currentType; }
    
    // Pointer to whatever we are currently testing
    punk_dsp::DSP::IDSPAlgorithm* getActiveDSP() { return activeDSP.get(); }

    // Callback so the Editor knows when to rebuild the UI
    std::function<void()> onAlgorithmChanged;

private:
    AlgorithmType currentType = AlgorithmType::None;
    std::unique_ptr<punk_dsp::DSP::IDSPAlgorithm> activeDSP;
    juce::dsp::ProcessSpec spec;
    
    // =============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WorkbenchProcessor)
};
