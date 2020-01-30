#ifndef FULLKNOBSCOMPONENT_H_INCLUDED
#define FULLKNOBSCOMPONENT_H_INCLUDED

#include "KnobsComponent.h"

class FullKnobsComponent : public KnobsComponent
{
public:
    FullKnobsComponent (AudioProcessor& p, AudioProcessorValueTreeState& vts, std::function<void()> paramLambda) :
        KnobsComponent (p, vts, paramLambda),
        name (vts.state.getType().toString().upToFirstOccurrenceOf ("Parameters", false, false))
    {
    }

    void paint (Graphics& g) override
    {
        KnobsComponent::paint (g);

        g.setColour (Colours::white);
        g.setFont (18.0f);
        Rectangle<int> area (5, 25, 80, 50);
        g.drawFittedText (name, area, Justification::left, 1);
    }

    void resized() override
    {
        int x = 80;
        bool first = true;
        for (auto* s : sliders)
        {
            int offset = first ? 0 : 15;
            s->slider.setBounds (x - offset, 15, 85, 80);
            x = s->slider.getRight();
            first = false;
        }

        for (auto* b : boxes)
        {
            int offset = first ? 0 : 0;
            b->box.setBounds (x - offset, 40, 70, 20);
            x = b->box.getRight();
            first = false;
        }

        for (auto* b : buttons)
        {
            int offset = first ? 0 : -10;
            b->button.setBounds (x - offset, 40, 70, 20);
            x = b->button.getRight();
            first = false;
        }
    }

private:
    const String name;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FullKnobsComponent)
};

#endif //FULLKNOBSCOMPONENT_H_INCLUDED
