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
    Element (float value, Type type) :
        idealValue (value),
        type (type)
    {
    
    }

    void setValue (float newValue) { idealValue = newValue; }
    void setAge (float ageInHours) { age = ageInHours; }
    void setTemp (float tempInKelvin) { temp = tempInKelvin; }
    void setCapFail (bool shouldFail) { capLifetime = shouldFail && type == Type::Cap ? getCapLifetime() : std::numeric_limits<float>::max(); }

    float getValue (float level = 0)
    {
        if (type == Type::Res)
        {
            float frac = (float) 1.51e12 * powf (age, 0.61f) * expf (-15087.0f / temp);
            return idealValue * (1.0f + frac);
        }

        if (type == Type::Cap)
        {
            if (age > capLifetime)
                return (deadCapRandom.nextFloat() * 15.0f*idealValue + 0.5f*idealValue) * (1.0f - 0.6f * level);

            return idealValue * (1.0f - 0.025f * log10f (age));
        }

        return idealValue;
    }

    float getCapLifetime()
    {
        float predictedLifetime = 10000.0f * powf (2.0f, (450.0f - temp) / 10.0f);

        std::default_random_engine engine;
        engine.seed (0x1234);
        std::normal_distribution<float> distribution (1.2f * predictedLifetime, 0.1f * predictedLifetime);
        
        return distribution (engine);
    }

private:
    float idealValue;
    const Type type;

    MyRandomGenerator generator;
    Random deadCapRandom;

    float capLifetime = std::numeric_limits<float>::max();
    float age = 1.0f; // hours
    float temp = 373.0f; // Kelvin

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Element)
};

#endif //ELEMENT_H_INCLUDED
