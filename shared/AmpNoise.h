#ifndef AMPNOISE_H_INCLUDED
#define AMPNOISE_H_INCLUDED

#include "JuceHeader.h"
#include <random>
#include <chrono>

class AmpNoise
{
public:
    AmpNoise() :
        gaussian (0.0f, 0.33f)
    {
        engine.seed (std::chrono::system_clock::now().time_since_epoch().count());
    }

    void setGain (float noiseGain)
    {
        gain.setTargetValue (noiseGain);
    }

    inline float getNoiseSample()
    {
        return gain.getNextValue() * gaussian (engine);    
    }

    /** Only use for freq response */
    inline float getNoiseMag()
    {
        return gain.getTargetValue() * gaussian (engine);    
    }

    void processBlock (float* buffer, const int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
            buffer[n] += getNoiseSample();
    }

private:
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> gain = 0.0000001f;

    std::default_random_engine engine;
    std::normal_distribution<float> gaussian;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpNoise)
};

#endif //AMPNOISE_H_INCLUDED
