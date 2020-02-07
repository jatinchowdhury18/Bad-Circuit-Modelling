#include "DiodeClipperViz.h"

namespace
{
    constexpr int size = 44100 / 30;
    constexpr float fs = 44100.0f;
    constexpr float freq = 100.0f;
    constexpr float gain = 1.0f;
}

DiodeClipperViz::DiodeClipperViz (DiodeClipperCircuit& circuitLeft,DiodeClipperCircuit& circuitRight,
                                  float& gainParam, float& outParam, double& leakRPar) :
    circuitL (circuitLeft),
    circuitR (circuitRight),
    gainParam (gainParam),
    outParam (outParam),
    leakRPar (leakRPar)
{
    startTimerHz (25);

    dryBuffer.setSize (2, size);

    for (int ch = 0; ch < 2; ++ch)
        for (int n = 0; n < size; ++n)
            dryBuffer.setSample (ch, n, gain * sinf (MathConstants<float>::twoPi * n * freq / fs));

    wetBuffer.setSize (2, size);
}

void DiodeClipperViz::timerCallback()
{
    updateCurve();
}

void DiodeClipperViz::updateCurve()
{
    wetBuffer.makeCopyOf (dryBuffer);

    for (int ch = 0; ch < 2; ++ch)
    {
        auto& circuit = ch == 0 ? circuitL : circuitR;

        diodeClipper[ch].reset ((double) fs, 1000.0);
        diodeClipper[ch].setCircuitElements (circuit.getR(), circuit.getC (gain), leakRPar);

        auto* buffer = wetBuffer.getWritePointer (0);
        for (int n = 0; n < size; ++n)
            buffer[n] = diodeClipper[ch].processSample (gainParam * buffer[n]) * outParam;
    }

    const auto yFactor = 0.7f;

    for (int ch = 0; ch < 2; ++ch)
    {
        curvePath[ch].clear();
        bool started = false;
        for (int n = size/2; n < size; ++n)
        {
            auto x = dryBuffer.getSample (0, n);
            auto y = wetBuffer.getSample (0, n);

            auto xDraw = (x + (gain/1.0f)) * (float) getWidth() / (gain*2.0f);
            auto yDraw = (float) getHeight() * (0.5f - yFactor * y);

            if (! started)
            {
                curvePath[ch].startNewSubPath (xDraw, yDraw);
                started = true;
            }
            else
            {
                curvePath[ch].lineTo (xDraw, yDraw);
            }
        }
    }

    repaint();
}

void DiodeClipperViz::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::red);
    g.strokePath (curvePath[0], PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));
        
    g.setColour (Colours::blue);
    g.strokePath (curvePath[1], PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));
}

void DiodeClipperViz::resized()
{
    updateCurve();
}
