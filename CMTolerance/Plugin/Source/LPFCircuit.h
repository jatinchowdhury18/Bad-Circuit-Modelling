#ifndef LPFCIRCUIT_H_INCLUDED
#define LPFCIRCUIT_H_INCLUDED

#include "Element.h"

class LPFCircuit
{
public:
    LPFCircuit() :
        R1 (33800.0f),
        R2 (33800.0f),
        C1 ((float) 4.7e-9),
        C2 ((float) 4.7e-9),
        R3 (1000.0f),
        R4 (1500.0f)
    {}

    void setTolerance (int tol)
    {
        Tolerance tolerance = static_cast<Tolerance> (tol);
        R1.setTolerance (tolerance);
        R2.setTolerance (tolerance);
        C1.setTolerance (tolerance);
        C2.setTolerance (tolerance);
        R3.setTolerance (tolerance);
        R4.setTolerance (tolerance);
    }

    void setFreq (float freq)
    {
        auto rVal = 1.0f / (freq * MathConstants<float>::twoPi * (float) 4.7e-9);
        R1.setValue (rVal);
        R2.setValue (rVal);
    }

    void setQ (float Q)
    {
        R4.setValue ((2.0f - (1.0f / Q)) * 1000.0f);
    }

    float getActualFreq()
    {
        return (1.0f / (MathConstants<float>::twoPi * sqrtf (R1.getValue() * R2.getValue() * C1.getValue() * C2.getValue())));
    }

    float getActualQ()
    {
        return 1.0f / (2.0f - (R4.getValue() / R3.getValue()));
    }

private:
    Element R1; 
    Element R2; 
    Element C1; 
    Element C2; 
    Element R3; 
    Element R4; 

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LPFCircuit)
};

#endif //LPFCIRCUIT_H_INCLUDED
