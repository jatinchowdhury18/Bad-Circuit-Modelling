#pragma once

#include "FreqViz.h"
#include "KnobsComponent.h"

template <class Processor>
class PluginEditor : public AudioProcessorEditor
{
public:
    PluginEditor (Processor&);

    void resized() override;

private:
    Processor& processor;

    FreqViz freqViz;
    KnobsComponent knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
