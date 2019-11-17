#include "FreqViz.h"

namespace
{
    constexpr float lowFreq = 10.0f;
    constexpr float highFreq = 22000.0f;
}

FreqViz::FreqViz (LPF& leftLPF, LPF& rightLPF) :
    leftLPF (leftLPF),
    rightLPF (rightLPF)
{
    updateCurve();
}

void FreqViz::updateCurve()
{
    curvePathLeft.clear();
    curvePathRight.clear();

    bool started = false;
    const float scaleFactor = (float) getHeight() / 8.0f / 8.0f;

    for (float x = 0.0f; x < getWidth(); x += 0.5f)
    {   
        auto freq = getFreqForX (x);
        auto traceMagLeft = Decibels::gainToDecibels (leftLPF.getMagnitudeAtFreq (freq));
        auto traceMagRight = Decibels::gainToDecibels (rightLPF.getMagnitudeAtFreq (freq));

        if (! started)
        {
            curvePathLeft.startNewSubPath (x, getHeight() / 2 - traceMagLeft * scaleFactor);
            curvePathRight.startNewSubPath (x, getHeight() / 2 - traceMagRight * scaleFactor);

            started = true;
        }
        else
        {
            curvePathLeft.lineTo (x, getHeight() / 2 - traceMagLeft * scaleFactor);
            curvePathRight.lineTo (x, getHeight() / 2 - traceMagRight * scaleFactor);
        }
    }

    repaint();
}

float FreqViz::getFreqForX (float xPos)
{
    auto normX = xPos / (float) getWidth();
    return lowFreq * powf (highFreq / lowFreq, normX);
}

void FreqViz::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::red);
    g.strokePath (curvePathLeft, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));

    g.setColour (Colours::blue);
    g.strokePath (curvePathRight, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));
}

void FreqViz::resized()
{
    updateCurve();
}
