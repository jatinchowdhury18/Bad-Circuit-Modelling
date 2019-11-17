#include "PluginEditor.h"

template <class Processor>
PluginEditor<Processor>::PluginEditor (Processor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    freqViz (p.getLPF (0), p.getLPF (1)),
    knobs (p, p.getVTS(), [this] { freqViz.updateCurve(); })
{
    setSize (500, 400);

    addAndMakeVisible (freqViz);
    addAndMakeVisible (knobs);
}

template <class Processor>
void PluginEditor<Processor>::resized()
{
    freqViz.setBounds (0, 0, getWidth(), getHeight() - 100);
    knobs.setBounds (0, getHeight() - 100, getWidth(), 100);
}
