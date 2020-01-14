#include "FullPluginEditor.h"

template <class Processor>
FullPluginEditor<Processor>::FullPluginEditor (Processor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    freqViz (p.getLPF (0), p.getLPF (1))
{
    auto& states = p.getVTS();

    for (auto state : states)
        knobs.add (std::make_unique<FullKnobsComponent> (p, *state, [this, &p] { p.updateParams(); freqViz.updateCurve(); }));

    addAndMakeVisible (freqViz);
    for (auto* k : knobs)
        addAndMakeVisible (k);

    setSize (400, 250 + 100 * knobs.size());
}

template <class Processor>
void FullPluginEditor<Processor>::resized()
{
    freqViz.setBounds (0, 0, getWidth(), getHeight() - 100 * knobs.size());

    for (int i = 0; i < knobs.size(); ++i)
        knobs[i]->setBounds (0, getHeight() - 100 * (knobs.size() - i), getWidth(), 100);
}
