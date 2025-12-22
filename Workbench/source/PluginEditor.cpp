#include "PluginEditor.h"

WorkbenchEditor::WorkbenchEditor (WorkbenchProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::ignoreUnused (audioProcessor);
    
    setSize (400, 500);

    // 1. Setup the Selector
    algoSelector.addItem("None", 1);
    algoSelector.addItem("Compressor", 2);
    addAndMakeVisible(algoSelector);

    algoSelector.onChange = [this] {
        audioProcessor.setAlgorithm(static_cast<WorkbenchAudioProcessor::AlgorithmType>(algoSelector.getSelectedItemIndex()));
    };

    // 2. Link the processor callback to our rebuild function
    audioProcessor.onAlgorithmChanged = [this] { rebuildUI(); };

    // Initial build
    rebuildUI();
}

WorkbenchEditor::~WorkbenchEditor()
{
}

void WorkbenchEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::royalblue);
}

void WorkbenchEditor::rebuildUI()
{
    sliders.clear();
    labels.clear();
    toggles.clear();

    auto* dsp = audioProcessor.getActiveDSP();
    if (!dsp) return;

    for (auto& p : dsp->getParameters())
    {
        if (p.type == ParameterInfo::Type::Boolean)
        {
            // --- Create a Toggle ---
            auto* t = toggles.add(std::make_unique<juce::ToggleButton>(p.name));
            t->setToggleState(p.defaultValue > 0.5f, juce::dontSendNotification);
            
            t->onClick = [dsp, p, t] {
                dsp->updateParameter(p.paramID, t->getToggleState() ? 1.0f : 0.0f);
            };
            addAndMakeVisible(t);
        }
        else
        {
            // --- Create a Slider (Existing Logic) ---
            auto* s = sliders.add(std::make_unique<juce::Slider>());
            // ... range and callback ...
        }
    }
    
    resized(); // Position the new items
}

void WorkbenchEditor::resized()
{
    algoSelector.setBounds(10, 10, 150, 30);
    
    int y = 60;
    for (auto* s : sliders)
    {
        s->setBounds(100, y, 250, 40);
        y += 60;
    }
}
