/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "FullPluginEditor.h"
#include "FullPluginEditor.cpp"

//==============================================================================
SallenKeyLpfAudioProcessor::SallenKeyLpfAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    vts.add (std::make_unique<AudioProcessorValueTreeState> (*this, nullptr, Identifier ("CircuitParameters"), createParameterLayout (0)));
    freqParam = vts[0]->getRawParameterValue ("cutoff_Hz");
    qParam = vts[0]->getRawParameterValue ("filtq_");

    vts.add (std::make_unique<AudioProcessorValueTreeState> (*this, nullptr, Identifier ("ComponentsParameters"), createParameterLayout (1)));


    vts.add (std::make_unique<AudioProcessorValueTreeState> (*this, nullptr, Identifier ("OpAmpParameters"), createParameterLayout (2)));
    oaTempParam = vts[2]->getRawParameterValue ("oa_temp_C");
    oaAgeParam = vts[2]->getRawParameterValue ("oa_age_years");
}

SallenKeyLpfAudioProcessor::~SallenKeyLpfAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout SallenKeyLpfAudioProcessor::createParameterLayout (int type)
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    if (type == 0) // normal parameters
    {
        NormalisableRange<float> freqRange (20.0f, 22000.0f);
        freqRange.setSkewForCentre (1000.0f);

        NormalisableRange<float> qRange (0.1f, 18.0f);
        qRange.setSkewForCentre (0.707f);

        params.push_back (std::make_unique<AudioParameterFloat> ("cutoff_Hz", "Cutoff", freqRange, 1000.0f));
        params.push_back (std::make_unique<AudioParameterFloat> ("filtq_", "Q", qRange, 0.707f));
    }
    else if (type == 1) // component parameters
    {
        // Tolerance
        StringArray choices ({ "Test", "test2" });
        params.push_back (std::make_unique<AudioParameterChoice> ("tol", "Tolerance", choices, 0)); // Element::getChoices(), 0));

        // Aging
        NormalisableRange<float> ageRange (0.0f, 500.0f);
        ageRange.setSkewForCentre (50.0f);

        params.push_back (std::make_unique<AudioParameterFloat> ("c_age_yrs", "Age", ageRange, 0.0f));
        params.push_back (std::make_unique<AudioParameterFloat> ("c_temp_C", "Operating Temp", 100.0f, 500.0f, 373.0f));
        params.push_back (std::make_unique<AudioParameterBool>  ("c_capfail", "Cap Fail", true));
    }
    else if (type == 2) // op amp parameters
    {
        params.push_back (std::make_unique<AudioParameterFloat> ("oa_temp_C", "Temperature", -200.0f, 100.0f, 25.0f));
        params.push_back (std::make_unique<AudioParameterFloat> ("oa_age_years", "Age", 0.0f, 100.0f, 0.0f));
    }

    return { params.begin(), params.end() };
}

//==============================================================================
const String SallenKeyLpfAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SallenKeyLpfAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SallenKeyLpfAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SallenKeyLpfAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SallenKeyLpfAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SallenKeyLpfAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SallenKeyLpfAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SallenKeyLpfAudioProcessor::setCurrentProgram (int index)
{
}

const String SallenKeyLpfAudioProcessor::getProgramName (int index)
{
    return {};
}

void SallenKeyLpfAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SallenKeyLpfAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int ch = 0; ch < 2; ++ch)
        lpf[ch].reset ((float) sampleRate);
}

void SallenKeyLpfAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SallenKeyLpfAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SallenKeyLpfAudioProcessor::updateParams()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        lpf[ch].setTempAndAge (*oaTempParam + 273.0f, *oaAgeParam);
        lpf[ch].setFreq (*freqParam);
        lpf[ch].setQ (*qParam);
    }
}

void SallenKeyLpfAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    updateParams();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        lpf[ch].processBlock (buffer.getWritePointer (ch), buffer.getNumSamples());
    }
}

//==============================================================================
bool SallenKeyLpfAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SallenKeyLpfAudioProcessor::createEditor()
{
    return new FullPluginEditor<SallenKeyLpfAudioProcessor> (*this);
}

//==============================================================================
void SallenKeyLpfAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ValueTree states ("Parameters");

    for (auto state : vts)
        states.addChild (state->state, -1, nullptr);

    std::unique_ptr<XmlElement> xml (states.createXml());
    copyXmlToBinary (*xml, destData);
}

void SallenKeyLpfAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        auto states = ValueTree::fromXml (*xmlState);

        for (int i = 0; i < states.getNumChildren(); ++i)
        {
            auto state = states.getChild (i);

            if (state.getType() == vts[i]->state.getType())
                vts[i]->replaceState (state);

        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SallenKeyLpfAudioProcessor();
}
