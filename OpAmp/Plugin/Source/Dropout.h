#ifndef DROPOUT_H_INCLUDED
#define DROPOUT_H_INCLUDED

#include "JuceHeader.h"

class Dropout
{
public:
    Dropout()
    {
        smoothA.reset (200);
    }

    void processBlock (float* buffer, const int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
            buffer[n] = processSample (buffer[n]);
    }

    inline float processSample (float x)
    {
        if (smoothA.isSmoothing())
            updateCoefs (smoothA.getNextValue());

        if (x > B)
            return x - B + cube (B/a);

        if (x < -B)
            return x + B - cube (B/a);

        return cube (x/a);
    }

    void setA (float newA)
    {
        if (newA != smoothA.getTargetValue())
            smoothA.setTargetValue (newA);
    }

private:
    inline void updateCoefs (float newA)
    {
        a = newA;
        B = sqrtf (cube (a) / 3.0f);
    }

    inline float cube (float x)
    {
        return x*x*x;
    }

    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> smoothA = 0.00001f;
    float a = 0.00001f;
    float B = sqrtf (cube (a) / 3.0f);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Dropout)
};

#endif //DROPOUT_H_INCLUDED
