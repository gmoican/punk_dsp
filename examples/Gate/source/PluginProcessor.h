#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "punk_dsp/punk_dsp.h"

#if (MSVC)
#include "ipps.h"
#endif

namespace Parameters
{
    // Ratio
    constexpr auto ratioId = "ratio";
    constexpr auto ratioName = "Ratio (:1)";
    constexpr auto ratioDefault = 4.0f;
    constexpr auto ratioMin = 1.0f;
    constexpr auto ratioMax = 100.0f;

    // Threshold
    constexpr auto thresId = "thres";
    constexpr auto thresName = "Threshold (dB)";
    constexpr auto thresDefault = -80.0f;
    constexpr auto thresMin = -100.0f;
    constexpr auto thresMax = 0.0f;

    // Knee width
    constexpr auto kneeId = "knee";
    constexpr auto kneeName = "Knee Width (dB)";
    constexpr auto kneeDefault = 12.0f;
    constexpr auto kneeMin = 1.0f;
    constexpr auto kneeMax = 30.0f;

    // Attack
    constexpr auto attackId = "attack";
    constexpr auto attackName = "Attack (ms)";
    constexpr auto attackDefault = 15.0f;
    constexpr auto attackMin = 0.1f;
    constexpr auto attackMax = 250.0f;

    // Release
    constexpr auto releaseId = "release";
    constexpr auto releaseName = "Release (ms)";
    constexpr auto releaseDefault = 60.0f;
    constexpr auto releaseMin = 5.0f;
    constexpr auto releaseMax = 3000.0f;

    // Comp mix control
    constexpr auto mixId = "mix";
    constexpr auto mixName = "Mix";
    constexpr auto mixDefault = 100.f;
    constexpr auto mixMin = 0.0f;
    constexpr auto mixMax = 100.0f;

}

class GateProcessor : public juce::AudioProcessor
{
public:
    GateProcessor();
    ~GateProcessor() override;

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
    
    // ===== MY STUFF ===============================================================
    juce::AudioProcessorValueTreeState apvts;
    
    void updateParameters();

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
    punk_dsp::Gate gate;
    
    // =============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GateProcessor)
};
