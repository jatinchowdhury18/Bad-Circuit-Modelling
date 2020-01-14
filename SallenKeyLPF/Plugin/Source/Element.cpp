#include "Element.h"

Element::Element (float value, Type type) :
    idealValue (value),
    type (type),
    tolDistribution (0.0f, 5.0f),
    pointOneFactor (getTolFactor (0.0f, 0.001f)),
    pointFiveFactor (getTolFactor (0.001f, 0.005f)),
    oneFactor (getTolFactor (0.005f, 0.01f)),
    fiveFactor (getTolFactor (0.01f, 0.05f)),
    tenFactor (getTolFactor (0.05f, 0.1f)),
    twentyFiveFactor (getTolFactor (0.1f, 0.25f))
{

}

void Element::setAgeCharacteristics (float ageInHours, float tempInKelvin, bool shouldFail)
{
    age = ageInHours;
    temp = tempInKelvin;
    capLifetime = shouldFail && type == Type::Cap ? getCapLifetime() : std::numeric_limits<float>::max();
}

float Element::getValue (float level)
{
    float value = getTolValue();

    if (type == Type::Res)
    {
        float frac = (float) 1.51e12 * powf (age, 0.61f) * expf (-15087.0f / temp);
        return value * (1.0f + frac);
    }

    if (type == Type::Cap)
    {
        if (age > capLifetime)
            return (deadCapRandom.nextFloat() * 15.0f*value + 0.5f*value) * (1.0f - 0.6f * level);

        return value * (1.0f - 0.025f * log10f (age));
    }

    return idealValue;
}

float Element::getCapLifetime()
{
    float predictedLifetime = 10000.0f * powf (2.0f, (450.0f - temp) / 10.0f);

    std::default_random_engine engine;
    engine.seed (0x1234);
    std::normal_distribution<float> distribution (1.2f * predictedLifetime, 0.1f * predictedLifetime);

    return distribution (engine);
}

float Element::getTolValue()
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

    if (tol == Tolerance::TwentyFive)
        return idealValue * (1.0f + twentyFiveFactor);

    return idealValue;
}

float Element::getTolFactor (float tol_low, float tol_high)
{
    float return_val = 10000000.0f;
    while (abs(return_val) > tol_high || abs(return_val) < tol_low)
        return_val = tolDistribution (generator);
    return return_val;
}

StringArray Element::getChoices()
{
    return StringArray ({ "Ideal", "0.1%", "0.5%", "1%", "5%", "10%", "25%" });
}
