#include "DiodeClipperEditor.h"

template <class Processor>
DiodeClipperEditor<Processor>::DiodeClipperEditor (Processor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    viz (p.getCircuit (0), p.getCircuit (1), p.getGainParam())
{
    auto& states = p.getVTS();

    for (auto state : states)
        knobs.add (std::make_unique<FullKnobsComponent> (p, *state, [this, &p] { p.updateParams(); viz.updateCurve(); }));

    addAndMakeVisible (viz);
    for (auto* k : knobs)
        addAndMakeVisible (k);

    setSize (400, 250 + 100 * knobs.size());
}

template <class Processor>
void DiodeClipperEditor<Processor>::resized()
{
    viz.setBounds (0, 0, getWidth(), getHeight() - 100 * knobs.size());

    for (int i = 0; i < knobs.size(); ++i)
        knobs[i]->setBounds (0, getHeight() - 100 * (knobs.size() - i), getWidth(), 100);
}
