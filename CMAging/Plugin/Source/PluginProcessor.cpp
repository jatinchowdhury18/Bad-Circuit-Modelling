/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginEditor.cpp"
#include "Element.h"

//==============================================================================
CmagingAudioProcessor::CmagingAudioProcessor()
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
    ageParam = vts.getRawParameterValue ("age_yrs");
    tempParam = vts.getRawParameterValue ("temp_K");
    capFailParam = vts.getRawParameterValue ("capfail");
}

CmagingAudioProcessor::~CmagingAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout CmagingAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    NormalisableRange<float> freqRange (20.0f, 22000.0f);
    freqRange.setSkewForCentre (1000.0f);

    NormalisableRange<float> qRange (0.1f, 18.0f);
    qRange.setSkewForCentre (0.707f);

    NormalisableRange<float> ageRange (0.0f, 500.0f);
    ageRange.setSkewForCentre (50.0f);

    params.push_back (std::make_unique<AudioParameterFloat> ("cutoff_Hz", "Cutoff", freqRange, 1000.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("filtq_", "Q", qRange, 0.707f));
    params.push_back (std::make_unique<AudioParameterFloat> ("age_yrs", "Age", ageRange, 0.0f));
    params.push_back (std::make_unique<AudioParameterFloat> ("temp_K", "Operating Temp", 100.0f, 500.0f, 373.0f));
    params.push_back (std::make_unique<AudioParameterBool>  ("capfail", "Cap Fail", true));

    return { params.begin(), params.end()  };
}

//==============================================================================
const String CmagingAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CmagingAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CmagingAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CmagingAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CmagingAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CmagingAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CmagingAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CmagingAudioProcessor::setCurrentProgram (int index)
{
}

const String CmagingAudioProcessor::getProgramName (int index)
{
    return {};
}

void CmagingAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void CmagingAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
        lpf[ch].reset ((float) sampleRate);
}

void CmagingAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CmagingAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void CmagingAudioProcessor::updateParams()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        lpfCircuit[ch].setAge (*ageParam);
        lpfCircuit[ch].setTemp (*tempParam);
        lpfCircuit[ch].setCapFail ((bool) *capFailParam);
        lpfCircuit[ch].setFreq (*freqParam);
        lpfCircuit[ch].setQ (*qParam);

        lpf[ch].setFreq (lpfCircuit[ch].getActualFreq());
        lpf[ch].setQ (lpfCircuit[ch].getActualQ());
    }
}

void CmagingAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    
    updateParams();

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        lpf[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }
}

//==============================================================================
bool CmagingAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CmagingAudioProcessor::createEditor()
{
    return new PluginEditor<CmagingAudioProcessor> (*this);
}

//==============================================================================
void CmagingAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto  state = vts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void CmagingAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
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
    return new CmagingAudioProcessor();
}
