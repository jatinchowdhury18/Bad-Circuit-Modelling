#ifndef DIODECLIPPEREDITOR_H_INCLUDED
#define DIODECLIPPEREDITOR_H_INCLUDED

#include "FullKnobsComponent.h"
#include "DiodeClipperViz.h"

template <class Processor>
class DiodeClipperEditor : public AudioProcessorEditor
{
public:
    DiodeClipperEditor (Processor&);

    void resized() override;

private:
    Processor& processor;

    DiodeClipperViz viz;
    OwnedArray<FullKnobsComponent> knobs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DiodeClipperEditor)
};

#endif //DIODECLIPPEREDITOR_H_INCLUDED
