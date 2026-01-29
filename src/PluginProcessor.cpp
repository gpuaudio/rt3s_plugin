#include "PluginProcessor.h"

//==============================================================================
SourceSeparationAudioProcessor::SourceSeparationAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",     juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output 1",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output 2",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output 3",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output 4",  juce::AudioChannelSet::stereo(), true)
                       )
#endif
{
    auto currentModulePath = ::GetCurrentModulePath();
    auto parentPath = currentModulePath.parent_path();
    auto paramsPath = parentPath / "params.bw";

    _proc = createGpuProcessor(paramsPath.string().c_str(), true);
}

SourceSeparationAudioProcessor::~SourceSeparationAudioProcessor()
{
}

//==============================================================================
const juce::String SourceSeparationAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SourceSeparationAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SourceSeparationAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SourceSeparationAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SourceSeparationAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SourceSeparationAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't like this returning 0
}

int SourceSeparationAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SourceSeparationAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SourceSeparationAudioProcessor::getProgramName (int index)
{
    return {};
}

void SourceSeparationAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SourceSeparationAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
}

void SourceSeparationAudioProcessor::releaseResources()
{
}

bool SourceSeparationAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // We strictly require 1 stereo input
    if (layouts.getMainInputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // We strictly require 4 stereo outputs (or however many the host enables, 
    // but checks they are stereo if enabled)
    for (int i = 0; i < layouts.outputBuses.size(); ++i)
    {
        if (layouts.outputBuses[i] != juce::AudioChannelSet::stereo() 
         && layouts.outputBuses[i] != juce::AudioChannelSet::disabled())
        {
            return false;
        }
    }

    return true;
}

void SourceSeparationAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int numSamples = buffer.getNumSamples();

    if (totalNumInputChannels >= 2 && totalNumOutputChannels >= 8)
    {
        const ::std::array inStereo {
            buffer.getReadPointer(0), buffer.getReadPointer(1)
        };

        const ::std::array outStereoX4 {
            buffer.getWritePointer(0), buffer.getWritePointer(1),
            buffer.getWritePointer(2), buffer.getWritePointer(3),
            buffer.getWritePointer(4), buffer.getWritePointer(5),
            buffer.getWritePointer(6), buffer.getWritePointer(7),
        };

        _proc->process(inStereo.data(), outStereoX4.data(), numSamples);
    }
}

//==============================================================================
// Editor Creation
//==============================================================================
juce::AudioProcessorEditor* SourceSeparationAudioProcessor::createEditor()
{
    // We return nullptr because hasEditor() returns false.
    return nullptr;
}

//==============================================================================
void SourceSeparationAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Store parameters here
}

void SourceSeparationAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore parameters here
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SourceSeparationAudioProcessor();
}
