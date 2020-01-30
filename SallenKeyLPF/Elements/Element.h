#ifndef ELEMENT_H_INCLUDED
#define ELEMENT_H_INCLUDED

#include "JuceHeader.h"
#include <random>
#include <chrono>

enum class Type
{
    Res,
    Cap,
};

enum class Tolerance
{
    Ideal,
    PointOne,
    PointFive,
    One,
    Five,
    Ten,
    TwentyFive
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
    Element (float value, Type type);

    void setValue (float newValue) { idealValue = newValue; }

    // aging
    void setAgeCharacteristics (float ageInHours, float tempInKelvin, bool shouldFail);
    float getValue (float level = 0);
    float getCapLifetime();

    // tolerance
    void setTolerance (Tolerance newTol) { tol = newTol; }
    float getTolValue();
    float getTolFactor (float tol_low, float tol_high);
    static StringArray getChoices();

private:
    float idealValue;
    
    MyRandomGenerator generator;

    // aging stuff
    Random deadCapRandom;
    const Type type;
    float capLifetime = std::numeric_limits<float>::max();
    float age = 1.0f; // hours
    float temp = 373.0f; // Kelvin

    // tolerance stuff
    std::normal_distribution<float> tolDistribution;
    const float pointOneFactor;
    const float pointFiveFactor;
    const float oneFactor;
    const float fiveFactor;
    const float tenFactor;
    const float twentyFiveFactor;
    Tolerance tol = Tolerance::Ideal;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Element)
};

#endif //ELEMENT_H_INCLUDED
