#include "PluginEditor.h"

PluginEditor::PluginEditor (CompressorProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p)
{
    juce::ignoreUnused (processorRef);
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLnF);
    
    // --- LAYOUT ---
    header.setColour (juce::TextButton::buttonColourId, UIConstants::background.lighter(0.5f)
                                                                               .withAlpha(0.25f)
                      );
    header.setEnabled(false);
    header.setButtonText ("Punk DSP - Compressor");
    addAndMakeVisible (header);
    
    params.setColour (juce::TextButton::buttonColourId, UIConstants::background.lighter(0.5f)
                                                                               .withAlpha(0.25f)
                      );
    params.setEnabled(false);
    addAndMakeVisible (params);
    
    // Ratio knob
    ratioSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ratioSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    ratioSlider.setRange(Parameters::ratioMin, Parameters::ratioMax, 0.1);
    ratioSlider.setValue(Parameters::ratioDefault);
    ratioSlider.setName("Ratio");
    addAndMakeVisible(ratioSlider);
    
    ratioAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::ratioId, ratioSlider);
    
    // Thres knob
    thresSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    thresSlider.setRange(Parameters::thresMin, Parameters::thresMax, 0.1);
    thresSlider.setValue(Parameters::thresDefault);
    thresSlider.setName("Thres");
    addAndMakeVisible(thresSlider);
    
    thresAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::thresId, thresSlider);
    
    // Knee knob
    kneeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    kneeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    kneeSlider.setRange(Parameters::kneeMin, Parameters::kneeMax, 0.1);
    kneeSlider.setValue(Parameters::kneeDefault);
    kneeSlider.setName("Knee");
    addAndMakeVisible(kneeSlider);
    
    kneeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::kneeId, kneeSlider);

    // Attack knob
    attackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    attackSlider.setRange(Parameters::attackMin, Parameters::attackMax, 0.1);
    attackSlider.setValue(Parameters::attackDefault);
    attackSlider.setName("Att");
    addAndMakeVisible(attackSlider);
    
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::attackId, attackSlider);

    // Release knob
    releaseSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    releaseSlider.setRange(Parameters::releaseMin, Parameters::releaseMax, 0.1);
    releaseSlider.setValue(Parameters::releaseDefault);
    releaseSlider.setName("Rel");
    addAndMakeVisible(releaseSlider);
    
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::releaseId, releaseSlider);

    // Makeup knob
    makeupSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    makeupSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    makeupSlider.setRange(Parameters::makeupMin, Parameters::makeupMax, 0.1);
    makeupSlider.setValue(Parameters::makeupDefault);
    makeupSlider.setName("Makeup");
    addAndMakeVisible(makeupSlider);
    
    makeupAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::makeupId, makeupSlider);
    
    // Mix knob
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setRange(Parameters::mixMin, Parameters::mixMax, 0.01);
    mixSlider.setValue(Parameters::mixDefault);
    mixSlider.setName("Mix");
    addAndMakeVisible(mixSlider);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::mixId, mixSlider);

    // Topology button
    feedButton.setClickingTogglesState(true);
    addAndMakeVisible(feedButton);
    
    feedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processorRef.apvts, Parameters::feedId, feedButton);
    
    // Gain Reduction Display
    grDisplay.setClickingTogglesState(false);
    addAndMakeVisible(grDisplay);
    
    setSize (650, 420);
}

PluginEditor::~PluginEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (UIConstants::background);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    
    // --- LAYOUT SETUP ---
    auto headerArea = area.removeFromTop( 50 );
    auto paramsArea = area.reduced( 20 );
    
    header.setBounds(headerArea);
    params.setBounds(paramsArea);
    
    // --- PARAMS LAYOUT ---
    auto paramsBounds = params.getLocalBounds().reduced(UIConstants::margin);
    
    // First row: 3 sliders
    auto row1 = paramsBounds.removeFromTop(UIConstants::knobSize + UIConstants::margin);
    ratioSlider.setBounds(row1.removeFromLeft(UIConstants::knobSize));
    row1.removeFromLeft(UIConstants::margin);
    thresSlider.setBounds(row1.removeFromLeft(UIConstants::knobSize));
    row1.removeFromLeft(UIConstants::margin);
    kneeSlider.setBounds(row1.removeFromLeft(UIConstants::knobSize));
    
    paramsBounds.removeFromTop(UIConstants::margin);
    
    // Second row: 3 sliders
    auto row2 = paramsBounds.removeFromTop(UIConstants::knobSize + UIConstants::margin);
    attackSlider.setBounds(row2.removeFromLeft(UIConstants::knobSize));
    row2.removeFromLeft(UIConstants::margin);
    releaseSlider.setBounds(row2.removeFromLeft(UIConstants::knobSize));
    row2.removeFromLeft(UIConstants::margin);
    makeupSlider.setBounds(row2.removeFromLeft(UIConstants::knobSize));
    
    paramsBounds.removeFromTop(UIConstants::margin);
    
    // Third row: 1 slider + 2 buttons
    auto row3 = paramsBounds.removeFromTop(UIConstants::knobSize + UIConstants::margin);
    feedButton.setBounds(row3.removeFromLeft(UIConstants::knobSize));
    row3.removeFromLeft(UIConstants::margin);
    mixSlider.setBounds(row3.removeFromLeft(UIConstants::knobSize));
    row3.removeFromLeft(UIConstants::margin);
    grDisplay.setBounds(row3.removeFromLeft(UIConstants::knobSize));
}
