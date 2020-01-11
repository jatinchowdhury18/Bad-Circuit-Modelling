#ifndef LPF1_H_INCLUDED
#define LPF1_H_INCLUDED

#include "JuceHeader.h"

/** First-order LPF */
class LPF1
{
public:
    LPF1() {}

    virtual void setFreq (float freq)
    {
        if (freq != fc.getTargetValue())
            fc.setTargetValue (jmin (freq, fs / 2.0f - 50.0f));
    }

    void reset (float sampleRate)
    {
        fs = sampleRate;

        for (int n = 0; n < 2; ++n)
            z[n] = 0.0f;

        calcCoefs (fc.skip (smoothSteps));
    }

    void calcCoefs (float curFc)
    {
        float wc = MathConstants<float>::twoPi * curFc / fs;
        float c = 1.0f / dsp::FastMathApproximations::tan (wc / 2.0f);
        float a0 = c + 1.0f;

        b[0] = 1.0f / a0;
        b[1] = b[0];
        a[1] = (1.0f - c) / a0;
    }

    void processBlock (float* buffer, const int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            if (fc.isSmoothing ())
                calcCoefs (fc.getNextValue());

            buffer[n] = processSample (buffer[n]);
        }
    }

    inline float processSample (float x)
    {
        float y = z[1] + x*b[0];
        z[1] = x*b[1] - y *a[1];
        return y;
    }

    float getMagnitudeAtFreq (float freq)
    {
        std::complex <float> s (0, freq / fc.getTargetValue()); // s = j (w / w0)
        auto numerator = 1.0f;
        auto denominator = s + 1.0f;
        return abs (numerator / denominator);
    }

private:
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> fc = 1000.0f;

    enum
    {
        smoothSteps = 200,
    };

    float fs = 44100.0f;

    float b[2] = { 1.0f, 0.0f };
    float a[2] = { 1.0f, 0.0f };
    float z[2] = { 1.0f, 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LPF1)
};

#endif //LPF1_H_INCLUDED
