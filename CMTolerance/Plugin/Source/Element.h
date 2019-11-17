#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED

#include "JuceHeader.h"
#include <random>
#include <chrono> 

enum class Tolerance
{
    Ideal,
    PointOne,
    PointFive,
    One,
    Five,
    Ten,
};

class MyRandomGenerator : public std::default_random_engine
{
public:
    MyRandomGenerator()
    {
        seed(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

class Element
{
public:
    Element (float value) :
        idealValue (value),
        distribution (0.0f, 5.0f),
        pointOneFactor (getFactor (0.0f, 0.001f)),
        pointFiveFactor (getFactor (0.001f, 0.005f)),
        oneFactor (getFactor (0.005f, 0.01f)),
        fiveFactor (getFactor (0.01f, 0.05f)),
        tenFactor (getFactor (0.05f, 0.1f))
    {
    
    }

    void setValue (float newValue) { idealValue = newValue; }
    void setTolerance (Tolerance newTol) { tol = newTol; }

    float getValue()
    {
        if (tol == Tolerance::Ideal)
            return idealValue;

        if (tol == Tolerance::PointOne)
            return idealValue * (1.0f + pointOneFactor);

        if (tol == Tolerance::PointFive)
            return idealValue * (1.0f + pointFiveFactor);

        if (tol == Tolerance::One)
            return idealValue * (1.0f + oneFactor);

        if (tol == Tolerance::Five)
            return idealValue * (1.0f + fiveFactor);

        if (tol == Tolerance::Ten)
            return idealValue * (1.0f + tenFactor);

        return idealValue;
    }

    float getFactor (float tol_low, float tol_high)
    {
        float return_val = 10000000.0f;
        while (abs(return_val) > tol_high || abs(return_val) < tol_low)
            return_val = distribution (generator);
        return return_val;
    }

    static StringArray getChoices()
    {
        return StringArray ({ "Ideal", "0.1%", "0.5%", "1%", "5%", "10%" });
    }

private:
    float idealValue;

    MyRandomGenerator generator;
    std::normal_distribution<float> distribution;

    const float pointOneFactor;
    const float pointFiveFactor;
    const float oneFactor;
    const float fiveFactor;
    const float tenFactor;

    Tolerance tol = Tolerance::Ideal;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Element)
};

#endif //ELEMENT_H_INCLUDED
