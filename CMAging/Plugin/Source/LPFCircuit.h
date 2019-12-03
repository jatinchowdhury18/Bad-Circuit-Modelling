#ifndef LPFCIRCUIT_H_INCLUDED
#define LPFCIRCUIT_H_INCLUDED

#include "Element.h"

class LPFCircuit
{
public:
    LPFCircuit() :
        R1 (33800.0f, Type::Res),
        R2 (33800.0f, Type::Res),
        C1 ((float) 4.7e-9, Type::Cap),
        C2 ((float) 4.7e-9, Type::Cap),
        R3 (1000.0f, Type::Res),
        R4 (1500.0f, Type::Res)
    {}

    void setAge (float ageInYears)
    {
        float ageInHours = jmax (ageInYears * 365.0f * 24.0f, 1.0f);

        R1.setAge (ageInHours);
        R2.setAge (ageInHours);
        C1.setAge (ageInHours);
        C2.setAge (ageInHours);
        R3.setAge (ageInHours);
        R4.setAge (ageInHours);
    }

    void setTemp (float tempInKelvin)
    {
        R1.setTemp (tempInKelvin);
        R2.setTemp (tempInKelvin);
        C1.setTemp (tempInKelvin);
        C2.setTemp (tempInKelvin);
        R3.setTemp (tempInKelvin);
        R4.setTemp (tempInKelvin);
    }

    void setCapFail (bool capShouldFail)
    {
        C1.setCapFail (capShouldFail);
        C2.setCapFail (capShouldFail);
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
