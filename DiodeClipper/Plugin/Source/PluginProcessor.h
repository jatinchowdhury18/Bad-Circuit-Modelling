/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DiodeClipperCircuit.h"
#include "DiodeClipper.h"

//==============================================================================
/**
*/
class DiodeClipperAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DiodeClipperAudioProcessor();
    ~DiodeClipperAudioProcessor();

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
    void updateParams();

    DiodeClipperCircuit& getCircuit (int ind) { return diodeCircuit[ind]; }
    float& getGainParam() { return curGain; }
    float& getOutParam() { return curOutGain; }

private:
    // general params
    float* freqParam = nullptr;
    float* gainDBParam = nullptr;
    float* outDBParam = nullptr;

    // components
    float* cTolParam;
    float* cAgeParam;
    float* cTempParam;
    float* cCapFailParam;

    float curGain = 1.0f;
    float oldGain = 1.0f;

    float curOutGain = 1.0f;
    float oldOutGain = 1.0f;

    OwnedArray<AudioProcessorValueTreeState> vts;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout (int type);

    DiodeClipperCircuit diodeCircuit[2];
    DiodeClipper diodeClipper[2];
    float rmsLevel[2] = { 0.0f, 0.0f };

    dsp::Oversampling<float> oversampling;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DiodeClipperAudioProcessor)
};
