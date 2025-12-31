#pragma once

#include "PluginProcessor.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (GateProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GateProcessor& processorRef;
    
    // Custom Look and Feel
    punk_dsp::ExamplesLnF myCustomLnF;
    
    // Layout utilities
    juce::TextButton header, params;
    
    // Sliders - Rotary knobs
    juce::Slider ratioSlider, thresSlider, kneeSlider, attackSlider, releaseSlider, mixSlider;
    
    juce::TextButton grDisplay { "GR (dB)" };
        
    // Attachments for linking sliders-parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttachment, thresAttachment, kneeAttachment, attackAttachment, releaseAttachment, mixAttachment;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
