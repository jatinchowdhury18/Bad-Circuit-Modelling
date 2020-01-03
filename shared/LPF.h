#ifndef LPF_H_INCLUDED
#define LPF_H_INCLUDED

#include "JuceHeader.h"

class LPF
{
public:
    LPF()
    {
        fc.reset (smoothSteps);
        Q.reset (smoothSteps);
    }

    virtual ~LPF() {}

    virtual void setFreq (float freq)
    {
        if (freq != fc.getTargetValue())
            fc.setTargetValue (jmin (freq, fs / 2.0f - 50.0f));
    }

    virtual void setQ (float newQ)
    {
        if (newQ != Q.getTargetValue())
            Q.setTargetValue (newQ);
    }

    void reset (float sampleRate)
    {
        fs = sampleRate;

        for (int n = 0; n < 3; ++n)
            z[n] = 0.0f;

        calcCoefs (fc.skip (smoothSteps), Q.skip (smoothSteps));
    }

    void calcCoefs (float curFc, float curQ)
    {
        float wc = MathConstants<float>::twoPi * curFc / fs;
        float c = 1.0f / dsp::FastMathApproximations::tan (wc / 2.0f);
        float phi = c * c;
        float K = c / curQ;
        float a0 = phi + K + 1.0f;

        b[0] = 1.0f / a0;
        b[1] = 2.0f * b[0];
        b[2] = b[0];
        a[1] = 2.0f * (1.0f - phi) / a0;
        a[2] = (phi - K + 1.0f) / a0;
    }

    virtual void processBlock (float* buffer, const int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            if (fc.isSmoothing () || Q.isSmoothing())
                calcCoefs (fc.getNextValue(), Q.getNextValue());

            buffer[n] = processSample (buffer[n]);
        }
    }

    inline float processSample (float x)
    {
        float y = z[1] + x*b[0];
        z[1] = z[2] + x*b[1] - y*a[1];
        z[2] = x*b[2] - y*a[2];
        return y;
    }

    virtual float getMagnitudeAtFreq (float freq)
    {
        std::complex <float> s (0, freq / fc.getTargetValue()); // s = j (w / w0)
        auto numerator = 1.0f;
        auto denominator = s * s + s / Q.getTargetValue() + 1.0f;
        return abs (numerator / denominator);
    }

protected:
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> fc = 1000.0f;
    SmoothedValue<float, ValueSmoothingTypes::Multiplicative> Q = 0.707f;

private:
    enum
    {
        smoothSteps = 200,
    };

    float fs = 44100.0f;

    float b[3] = { 1.0f, 0.0f, 0.0f };
    float a[3] = { 1.0f, 0.0f, 0.0f };
    float z[3] = { 1.0f, 0.0f, 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LPF)
};

#endif //LPF_H_INCLUDED
