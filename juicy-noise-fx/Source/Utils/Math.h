
#pragma once

#include <cmath>
#include <random>

float stripInt(float value)
{
    float dummy;
    return std::modf(value, &dummy);
}

int sumDigits(int number)
{
    int sum = 0;

    while (number != 0)
    {
        int digit = number % 10;
        sum += digit;
        number /= 10;
    }

    return sum;
}

float randomFloat(float from, float to)
{
    return from + static_cast<float>(rand()) / (RAND_MAX / (to - from));
}

int randomInt(int from, int to)
{
    return from + rand() / (RAND_MAX / (to - from));
}

double sawtooth(float frequency, float amplitude, float time)
{
    const float period = 1.0f / frequency;
    const float phase = static_cast<float>(fmod(time, period)) / period;

    const float value = (2.0f * (phase * (phase * 0.5f)) - 1.0f) * amplitude;

    return value;
}

float sineWave(const int &sampleRate, float frequency, float amplitude, float time)
{
    constexpr float doublePi = 2 * M_PI;

    const float deltaThetaMultiplier = doublePi / static_cast<float>(sampleRate);
    const float deltaTheta = frequency * deltaThetaMultiplier;

    const float theta = fmod(time * deltaTheta, doublePi);
    const float currentSample = amplitude * sinf(theta);

    return fmod(currentSample, 1.0f);
}

float map(float value, float fromStart, float fromEnd, float toStart, float toEnd)
{
    const float normalizedValue = (value - fromStart) / (fromEnd - fromStart);
    const float mappedValue = normalizedValue * (toEnd - toStart) + toStart;

    return mappedValue;
}
