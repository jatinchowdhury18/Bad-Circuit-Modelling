#ifndef LPFCIRCUIT_H_INCLUDED
#define LPFCIRCUIT_H_INCLUDED

#include "CapNoise.h"

class LPFCircuit
{
public:
    LPFCircuit() {}

    inline void addNoise (float& y)
    {
        y += capNoise[0].getNoise() + capNoise[1].getNoise() * (R1 + R2) / R1;
    }

    void setQ (float Q)
    {
        R2 = (2.0f - (1.0f / Q)) * R1;
    }

    /** Temp in Kelvin */
    void setTemp (float temp)
    {
        for (int i = 0; i < 2; ++i)
            capNoise[i].setTemp (temp);
    }

private:
    const float R1 = 1000.0f;
    float R2 = 1500.0f;
    float C = (float) 4.7e-9;

    CapNoise capNoise[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LPFCircuit)
};

#endif //LPFCIRCUIT_H_INCLUDED
