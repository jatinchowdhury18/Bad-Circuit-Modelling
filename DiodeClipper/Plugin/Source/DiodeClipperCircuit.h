#ifndef DIODECLIPPERCIRCUIT_H_INCLUDED
#define DIODECLIPPERCIRCUIT_H_INCLUDED

#include "Element.h"

class DiodeClipperCircuit
{
public:
    DiodeClipperCircuit() :
        R1 (4700.0f, Type::Res),
        C1 ((float) 47.0e-9, Type::Cap)
    {}

    void setAgeCharacteristics (float ageInYears, float tempInKelvin, bool capFail)
    {
        float ageInHours = jmax (ageInYears * 365.0f * 24.0f, 1.0f);

        R1.setAgeCharacteristics (ageInHours, tempInKelvin, capFail);
        C1.setAgeCharacteristics (ageInHours, tempInKelvin, capFail);
    }

    void setTolerance (int tol)
    {
        Tolerance tolerance = static_cast<Tolerance> (tol);
        R1.setTolerance (tolerance);
        C1.setTolerance (tolerance);
    }

    void setFreq (float freq)
    {
        auto res = 1.0f / (MathConstants<float>::twoPi * freq * (float) 47.0e-9);
        R1.setValue (res);
    }

    float getR() { return R1.getValue(); }
    float getC() { return C1.getValue(); }

private:
    Element R1;
    Element C1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DiodeClipperCircuit)
};

#endif //DIODECLIPPERCIRCUIT_H_INCLUDED
