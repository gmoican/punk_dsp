#pragma once

#include "PluginProcessor.h"

//==============================================================================
class WorkbenchEditor : public juce::AudioProcessorEditor
{
public:
    explicit WorkbenchEditor (WorkbenchProcessor&);
    ~WorkbenchEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void rebuildUI();

    WorkbenchProcessor& audioProcessor;
    juce::ComboBox algoSelector;
    
    // Storage for dynamically created sliders
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> labels;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WorkbenchEditor)
};
