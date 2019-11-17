#pragma once

#include "JuceHeader.h"
#include "LPF.h"

class FreqViz : public Component
{
public:
    FreqViz (LPF& leftLPF, LPF& rightLPF);

    void updateCurve();
    float getFreqForX (float xPos);
    float getXForFreq (float freq);

    void paint (Graphics& g) override;
    void resized() override;

private:
    LPF& leftLPF;
    LPF& rightLPF;

    Path curvePathLeft;
    Path curvePathRight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FreqViz)
};
