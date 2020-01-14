#pragma once

#include "FreqViz.h"
#include "FullKnobsComponent.h"

template <class Processor>
class FullPluginEditor : public AudioProcessorEditor
{
public:
    FullPluginEditor (Processor&);

    void resized() override;

private:
    Processor& processor;

    FreqViz freqViz;
    OwnedArray<FullKnobsComponent> knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FullPluginEditor)
};
