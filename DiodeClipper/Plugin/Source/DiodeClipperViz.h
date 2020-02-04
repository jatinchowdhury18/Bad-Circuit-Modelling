#ifndef DIODECLIPPERVIZ_H_INCLUDED
#define DIODECLIPPERVIZ_H_INCLUDED

#include "JuceHeader.h"
#include "DiodeClipper.h"
#include "DiodeClipperCircuit.h"

class DiodeClipperViz : public Component,
                        private Timer
{
public:
    DiodeClipperViz (DiodeClipperCircuit& circuitLeft, DiodeClipperCircuit& circuitRight, float& gainParam, float& outParam);

    void timerCallback() override;

    void updateCurve();

    void paint (Graphics& g) override;
    void resized() override;

private:
    DiodeClipper diodeClipper[2];

    AudioBuffer<float> dryBuffer;
    AudioBuffer<float> wetBuffer;

    Path curvePath[2];

    DiodeClipperCircuit& circuitL;
    DiodeClipperCircuit& circuitR;
    float& gainParam;
    float& outParam;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DiodeClipperViz)
};

#endif //DIODECLIPPERVIZ_H_INCLUDED
