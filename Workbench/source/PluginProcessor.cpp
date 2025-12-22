#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WorkbenchProcessor::WorkbenchProcessor()
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  )
{
}

WorkbenchProcessor::~WorkbenchProcessor()
{
}

//==============================================================================
const juce::String WorkbenchProcessor::getName() const
{
    return JucePlugin_Name;
}

bool WorkbenchProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool WorkbenchProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool WorkbenchProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double WorkbenchProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WorkbenchProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int WorkbenchProcessor::getCurrentProgram()
{
    return 0;
}

void WorkbenchProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String WorkbenchProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void WorkbenchProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void WorkbenchProcessor::setAlgorithm(AlgorithmType type)
{
    if (currentType == type) return;

    // 1. Create the new DSP instance
    if (type == AlgorithmType::Compressor)
        activeDSP = std::make_unique<punk_dsp::DSP::Compressor>();
    else
        activeDSP = nullptr;

    currentType = type;

    // 2. Prepare the new DSP object if we are already playing
    if (activeDSP && spec.sampleRate > 0)
    {
        activeDSP->prepare(spec);
    }

    // 3. Tell the Editor to refresh
    if (onAlgorithmChanged) onAlgorithmChanged();
}

void WorkbenchProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    if (activeDSP) activeDSP->prepare(spec);
}

void WorkbenchProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool WorkbenchProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void WorkbenchProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Clear any unused output channel
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    if (activeDSP)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        activeDSP->process(block);
    }
}

//==============================================================================
bool WorkbenchProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* WorkbenchProcessor::createEditor()
{
    return new PluginEditor (*this);
    // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void WorkbenchProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void WorkbenchProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WorkbenchProcessor();
}
