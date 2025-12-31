#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GateProcessor::GateProcessor()
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ), apvts(*this, nullptr, "Parameters", createParams())
{
}

GateProcessor::~GateProcessor()
{
}

//==============================================================================
const juce::String GateProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GateProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GateProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GateProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double GateProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GateProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int GateProcessor::getCurrentProgram()
{
    return 0;
}

void GateProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String GateProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void GateProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

// =========== PARAMETER LAYOUT ====================
juce::AudioProcessorValueTreeState::ParameterLayout GateProcessor::createParams()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Ratio (:1)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::ratioId,
                                                           Parameters::ratioName,
                                                           juce::NormalisableRange<float>(Parameters::ratioMin, Parameters::ratioMax, 0.1f),
                                                           Parameters::ratioDefault
                                                           )
               );
    
    // Threshold (dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::thresId,
                                                           Parameters::thresName,
                                                           juce::NormalisableRange<float>(Parameters::thresMin, Parameters::thresMax, 0.1f),
                                                           Parameters::thresDefault
                                                           )
               );
    
    // Knee Width (dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::kneeId,
                                                           Parameters::kneeName,
                                                           juce::NormalisableRange<float>(Parameters::kneeMin, Parameters::kneeMax, 0.1f),
                                                           Parameters::kneeDefault
                                                           )
               );
    
    // Attack Time (ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::attackId,
                                                           Parameters::attackName,
                                                           juce::NormalisableRange<float>(Parameters::attackMin, Parameters::attackMax, 0.1f),
                                                           Parameters::attackDefault
                                                           )
               );
    
    // Release Time (ms)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::releaseId,
                                                           Parameters::releaseName,
                                                           juce::NormalisableRange<float>(Parameters::releaseMin, Parameters::releaseMax, 0.1f),
                                                           Parameters::releaseDefault
                                                           )
               );
    
    // Mix (%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
                                                           Parameters::mixId,
                                                           Parameters::mixName,
                                                           juce::NormalisableRange<float>(Parameters::mixMin, Parameters::mixMax, 1.0f),
                                                           Parameters::mixDefault
                                                           )
               );
        
    return layout;
}

//==============================================================================
void GateProcessor::updateParameters()
{
    gate.updateRatio( apvts.getRawParameterValue(Parameters::ratioId)->load() );
    gate.updateThres( apvts.getRawParameterValue(Parameters::thresId)->load() );
    gate.updateKnee( apvts.getRawParameterValue(Parameters::kneeId)->load() );
    
    gate.updateAttack( apvts.getRawParameterValue(Parameters::attackId)->load() );
    gate.updateRelease( apvts.getRawParameterValue(Parameters::releaseId)->load() );
            
    gate.updateMix( apvts.getRawParameterValue(Parameters::mixId)->load() );
}

void GateProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    gate.prepare(spec);
}

void GateProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool GateProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}

void GateProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Clear any unused output channel
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Update params
    updateParameters();
    
    // Process
    gate.process(buffer);
}

//==============================================================================
bool GateProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GateProcessor::createEditor()
{
    // return new PluginEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void GateProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void GateProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GateProcessor();
}
