/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "OpAmpLPF.h"

//==============================================================================
/**
*/
class SallenKeyLpfAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SallenKeyLpfAudioProcessor();
    ~SallenKeyLpfAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    OwnedArray<AudioProcessorValueTreeState>& getVTS() { return vts; }
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout (int type);

    LPF& getLPF (int ch) { return lpf[ch]; }
    void updateParams();

private:
    // circuit
    float* freqParam;
    float* qParam;

    // components

    // op amp
    float* oaTempParam;
    float* oaAgeParam;

    OpAmpLPF lpf[2];

    OwnedArray<AudioProcessorValueTreeState> vts;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SallenKeyLpfAudioProcessor)
};
