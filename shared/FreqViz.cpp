#include "FreqViz.h"

namespace
{
    constexpr float lowFreq = 10.0f;
    constexpr float highFreq = 22000.0f;
    constexpr float dashLengths[2] = { 4, 1 };
}

FreqViz::FreqViz (LPF& leftLPF, LPF& rightLPF) :
    leftLPF (leftLPF),
    rightLPF (rightLPF)
{
    startTimerHz (20);

    updateCurve();
}

void FreqViz::timerCallback()
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

float FreqViz::getXForFreq (float freq)
{
    auto normX = logf (freq / lowFreq) / logf (highFreq / lowFreq);
    return normX * (float) getWidth();
}

void FreqViz::paint (Graphics& g)
{
    g.fillAll (Colours::white);

    g.setColour (Colours::red);
    g.strokePath (curvePathLeft, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));

    g.setColour (Colours::blue);
    g.strokePath (curvePathRight, PathStrokeType (2.0f, PathStrokeType::JointStyle::curved));

    g.setColour (Colours::grey.withAlpha (0.75f));
    auto drawHorizontalLine = [this, &g] (float height)
    {
        Line<float> line (0.0f, height, (float) getWidth(), height);
        g.drawDashedLine (line, dashLengths, 2);
    };

    auto yFrac = 6.0f;
    for (float y = 1.0f; y < yFrac; ++y)
        drawHorizontalLine (y * (float) getHeight() / yFrac);

    float freqsToDraw[] = { 20.0f, 50.0f, 100.0f, 200.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f, 20000.0f };
    auto drawVerticalLine = [this, &g] (float freq)
    {
        auto x = getXForFreq (freq);
        Line<float> line (x, 0.0f, x, (float) getHeight());
        g.drawDashedLine (line, dashLengths, 2);
    };

    for (auto freq : freqsToDraw)
        drawVerticalLine (freq);
}

void FreqViz::resized()
{
    updateCurve();
}
