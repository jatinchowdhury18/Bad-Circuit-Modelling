/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DiodeClipperEditor.h"
#include "DiodeClipperEditor.cpp"

//==============================================================================
DiodeClipperAudioProcessor::DiodeClipperAudioProcessor()
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
    oversampling (2, 1, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR)
{
    vts.add (std::make_unique<AudioProcessorValueTreeState> (*this, nullptr, Identifier ("CircuitParameters"), createParameterLayout (0)));
    freqParam   = vts[0]->getRawParameterValue ("cutoff_Hz");
    gainDBParam = vts[0]->getRawParameterValue ("gain_dB");
    outDBParam  = vts[0]->getRawParameterValue ("out_dB");

    vts.add (std::make_unique<AudioProcessorValueTreeState> (*this, nullptr, Identifier ("ComponentsParameters"), createParameterLayout (1)));
    cTolParam = vts[1]->getRawParameterValue ("c_tol");
    cAgeParam = vts[1]->getRawParameterValue ("c_age_yrs");
    cTempParam = vts[1]->getRawParameterValue ("c_temp_C");
    cCapFailParam = vts[1]->getRawParameterValue ("c_capfail");
    cCapLeakParam = vts[1]->getRawParameterValue ("c_capleak_");
}

DiodeClipperAudioProcessor::~DiodeClipperAudioProcessor()
{
}

AudioProcessorValueTreeState::ParameterLayout DiodeClipperAudioProcessor::createParameterLayout (int type)
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    if (type == 0) // normal parameters
    {
        NormalisableRange<float> freqRange (20.0f, 22000.0f);
        freqRange.setSkewForCentre (1000.0f);

        params.push_back (std::make_unique<AudioParameterFloat> ("cutoff_Hz", "Cutoff", freqRange, 1000.0f));
        params.push_back (std::make_unique<AudioParameterFloat> ("gain_dB", "Gain", 0.0f, 30.0f, 0.0f));
        params.push_back (std::make_unique<AudioParameterFloat> ("out_dB", "Output", -30.0f, 30.0f, 0.0f));
    }
    else if (type == 1) // component parameters
    {
        // Tolerance
        params.push_back (std::make_unique<AudioParameterChoice> ("c_tol", "Tolerance", Element::getChoices(), 0));

        // Aging
        NormalisableRange<float> ageRange (0.0f, 500.0f);
        ageRange.setSkewForCentre (50.0f);

        params.push_back (std::make_unique<AudioParameterFloat> ("c_age_yrs", "Age", ageRange, 0.0f));
        params.push_back (std::make_unique<AudioParameterFloat> ("c_temp_C", "Temp.", -200.0f, 200.0f, 25.0f));
        params.push_back (std::make_unique<AudioParameterFloat> ("c_capleak_", "Cap Leak", 0.0f, 1.0f, 0.0f));
        params.push_back (std::make_unique<AudioParameterBool>  ("c_capfail", "Cap Fail", true));
    }

    return { params.begin(), params.end() };
}

//==============================================================================
const String DiodeClipperAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DiodeClipperAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DiodeClipperAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DiodeClipperAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DiodeClipperAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DiodeClipperAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DiodeClipperAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DiodeClipperAudioProcessor::setCurrentProgram (int index)
{
}

const String DiodeClipperAudioProcessor::getProgramName (int index)
{
    return {};
}

void DiodeClipperAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DiodeClipperAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing (samplesPerBlock);

    for (int ch = 0; ch < 2; ++ch)
        diodeClipper[ch].reset (sampleRate, (double) *freqParam);

    curGain = Decibels::decibelsToGain (*gainDBParam);
    oldGain = curGain;

    curOutGain = Decibels::decibelsToGain (*outDBParam);
    oldOutGain = curOutGain;
}

void DiodeClipperAudioProcessor::releaseResources()
{
    oversampling.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DiodeClipperAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void DiodeClipperAudioProcessor::updateParams()
{
    leakRPar = 100.0e6 * pow (1.0 - *cCapLeakParam, 0.5) + 1.2e3;

    for (int ch = 0; ch < 2; ++ch)
    {
        diodeCircuit[ch].setTolerance ((int) *cTolParam);
        diodeCircuit[ch].setAgeCharacteristics (*cAgeParam, *cTempParam + 273.0f, (bool) *cCapFailParam);
        diodeCircuit[ch].setFreq (*freqParam);

        diodeClipper[ch].setCircuitElements (diodeCircuit[ch].getR(), diodeCircuit[ch].getC (rmsLevel[ch]), leakRPar);
    }

    curGain = Decibels::decibelsToGain (*gainDBParam);
    curOutGain = Decibels::decibelsToGain (*outDBParam);
}

void DiodeClipperAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;

    updateParams();

    // Input gain stage
    if (oldGain == curGain)
    {
        buffer.applyGain (curGain);
    }
    else
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), oldGain, curGain);
        oldGain = curGain;
    }

    // WDF
    dsp::AudioBlock<float> block (buffer);
    dsp::AudioBlock<float> osBlock (buffer);

    osBlock = oversampling.processSamplesUp (block);

    float* ptrArray[] = {osBlock.getChannelPointer (0), osBlock.getChannelPointer (1)};
    AudioBuffer<float> osBuffer (ptrArray, 2, static_cast<int> (osBlock.getNumSamples()));

    for (int ch = 0; ch < osBuffer.getNumChannels(); ++ch)
    {
        auto* x = osBuffer.getWritePointer (ch);

        for (int n = 0; n < osBuffer.getNumSamples(); ++n)
        {
            x[n] = diodeClipper[ch].processSample (x[n]);

            if (std::isnan (x[n]))
                x[n] = 0.0f;
        }
    }

    oversampling.processSamplesDown (block);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        rmsLevel[ch] = buffer.getRMSLevel (ch, 0, buffer.getNumSamples());

    // Output gain stage
    if (oldOutGain == curOutGain)
    {
        buffer.applyGain (curOutGain);
    }
    else
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), oldOutGain, curOutGain);
        oldOutGain = curOutGain;
    }
}

//==============================================================================
bool DiodeClipperAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DiodeClipperAudioProcessor::createEditor()
{
    return new DiodeClipperEditor<DiodeClipperAudioProcessor> (*this);
}

//==============================================================================
void DiodeClipperAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    ValueTree states ("Parameters");

    for (auto state : vts)
        states.addChild (state->state, -1, nullptr);

    std::unique_ptr<XmlElement> xml (states.createXml());
    copyXmlToBinary (*xml, destData);
}

void DiodeClipperAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        auto states = ValueTree::fromXml (*xmlState);

        for (int i = 0; i < states.getNumChildren() && i < vts.size(); ++i)
        {
            auto state = states.getChild (i);

            if (! state.isValid() || ! vts[i]->state.isValid())
                continue;

            if (state.getType() == vts[i]->state.getType())
                vts[i]->replaceState (state);

        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DiodeClipperAudioProcessor();
}
