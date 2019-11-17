/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginEditor.cpp"

//==============================================================================
CmtoleranceAudioProcessor::CmtoleranceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    vts (*this, nullptr, Identifier ("Parameters"), createParameterLayout())
{
    freqParam = vts.getRawParameterValue ("cutoff_Hz");
    qParam = vts.getRawParameterValue ("filtq_");
}

CmtoleranceAudioProcessor::~CmtoleranceAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout CmtoleranceAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    NormalisableRange<float> freqRange (20.0f, 22000.0f);
    freqRange.setSkewForCentre (1000.0f);

    NormalisableRange<float> qRange (0.1f, 18.0f);
    qRange.setSkewForCentre (0.707f);

    params.push_back (std::make_unique<AudioParameterFloat> ("cutoff_Hz", "Cutoff", freqRange, 1000.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("filtq_", "Q", qRange, 0.707f));

    return { params.begin(), params.end() };
}

//==============================================================================
const String CmtoleranceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CmtoleranceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CmtoleranceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CmtoleranceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CmtoleranceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CmtoleranceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CmtoleranceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CmtoleranceAudioProcessor::setCurrentProgram (int index)
{
}

const String CmtoleranceAudioProcessor::getProgramName (int index)
{
    return {};
}

void CmtoleranceAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void CmtoleranceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
        lpf[ch].reset ((float) sampleRate);
}

void CmtoleranceAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CmtoleranceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CmtoleranceAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        lpf[ch].setFreq (*freqParam);
        lpf[ch].setQ (*qParam);

        lpf[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }
}

//==============================================================================
bool CmtoleranceAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CmtoleranceAudioProcessor::createEditor()
{
    return new PluginEditor<CmtoleranceAudioProcessor> (*this);
}

//==============================================================================
void CmtoleranceAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto  state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CmtoleranceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (vts.state.getType()))
            vts.replaceState (ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CmtoleranceAudioProcessor();
}
