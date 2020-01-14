#ifndef OPAMPLPF_H_INCLUDED
#define OPAMPLPF_H_INCLUDED

#include "LPF.h"
#include "LPF1.h"
#include "AmpNoise.h"
#include "Dropout.h"

class OpAmpLPF : public LPF
{
public:
    OpAmpLPF() {}

    void setTempAndAge (float tempK, float ageYrs)
    {
        freq = (30.968f * tempK) / (tempK + 290.48f) * 1000.0f * jmin(1.0f, 4.473f * expf(-0.0749f * ageYrs));
        noiseDB = ((105.07f * ageYrs) / (ageYrs + 8.6006f)) - 135.0f;
        dropoutAValue = 1.00001f - jmin(1.0f, 4.473f * expf(-0.0749f * ageYrs));
    }

    void reset (float sampleRate) override
    {
        LPF::reset (sampleRate);
        lpf1.reset (sampleRate);
    }

    void processBlock (float* buffer, const int numSamples) override
    {
        dropout.setA (dropoutAValue);
        ampNoise.setGain (Decibels::decibelsToGain (noiseDB));
        lpf1.setFreq (freq);

        dropout.processBlock (buffer, numSamples);
        LPF::processBlock (buffer, numSamples);
        ampNoise.processBlock (buffer, numSamples);
        lpf1.processBlock (buffer, numSamples);
    }

    float getMagnitudeAtFreq (float f) override
    {
        return LPF::getMagnitudeAtFreq (f) * lpf1.getMagnitudeAtFreq (f) * (1.0f + 10.0f * ampNoise.getNoiseMag());
    }

private:
    Dropout dropout;
    LPF1 lpf1;
    AmpNoise ampNoise;

    float freq = 20000.0f;
    float noiseDB = -120.0f;
    float dropoutAValue = 0.00001;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpAmpLPF)
};

#endif //OPAMPLPF_H_INCLUDED
