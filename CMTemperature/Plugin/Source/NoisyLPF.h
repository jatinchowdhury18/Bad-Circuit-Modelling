#ifndef NOISYLPF_H_INCLUDED
#define NOISYLPF_H_INCLUDED

#include "LPF.h"
#include "LPFCircuit.h"

class NoisyLPF : public LPF
{
public:
    NoisyLPF() {}

    void setQ (float newQ) override
    {
        LPF::setQ (newQ);
        lpfCircuit.setQ (newQ);
    }

    void setTemp (float tempK)
    {
        lpfCircuit.setTemp (tempK);
    }

    void processBlock (float* buffer, const int numSamples) override
    {
        for (int n = 0; n < numSamples; ++n)
        {
            if (fc.isSmoothing () || Q.isSmoothing())
                calcCoefs (fc.getNextValue(), Q.getNextValue());

            buffer[n] = processSample (buffer[n]);
            lpfCircuit.addNoise (buffer[n]);
        }
    }

    float getMagnitudeAtFreq (float freq) override
    {
        std::complex <float> s (0, freq / fc.getTargetValue()); // s = j (w / w0)
        auto numerator = 1.0f;
        auto denominator = s * s + s / Q.getTargetValue() + 1.0f;

        lpfCircuit.setQ (Q.getTargetValue());
        float mag1 = abs (numerator / denominator);
        float mag2 = mag1;
        lpfCircuit.addNoise (mag2);

        if (mag2 < 0.0f || mag2 > 20.0f)
            mag2 = mag1;

        return mag2;
    }

private:
    LPFCircuit lpfCircuit;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NoisyLPF)
};

#endif //NOISYLPF_H_INCLUDED
