#ifndef CAPNOISE_H_INCLUDED
#define CAPNOISE_H_INCLUDED

#include "JuceHeader.h"
#include <random>
#include <chrono>

class CapNoise
{
public:
    CapNoise()
    {
        engine.seed (std::chrono::system_clock::now().time_since_epoch().count());
        gaussian.reset (new std::normal_distribution<float> (0.0f, sqrtf (boltzmann * 10.0f * C)));
    }

    inline float getNoise()
    {
        float Q = gaussian->operator() (engine);
        return Q / C;
    }

    /** Temp in Kelvin */
    void setTemp (float temp)
    {
        gaussian.reset (new std::normal_distribution<float> (0.0f, sqrtf (boltzmann * temp * C)));
    }

private:
    float C = (float) 4.7e-9;
    const float boltzmann = (float) 1.3806e-23;

    std::default_random_engine engine;
    std::unique_ptr<std::normal_distribution<float>> gaussian;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CapNoise)
};

#endif //CAPNOISE_H_INCLUDED
