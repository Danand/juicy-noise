
#pragma once

#include <cmath>
#include <random>

float magnitude(float x, float y, float z)
{
    return sqrt(
        (x * x) +
        (y * y) +
        (z * z));
}

float lerp(float from, float to, float ratio)
{
    return std::abs((1.0f - ratio) * from + (to * ratio));
}

float inverseLerp(float from, float to, float value)
{
    return std::abs((value - from) / (to - from));
}

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

float sawtoothWave(float time, int frequency, float amplitude, float phaseShift)
{
    float period = 1.0f / frequency;
    float cycles = (time - phaseShift) / period;
    float normalizedTime = cycles - std::floor(cycles);
    float amplitudeAtTime = amplitude * (2.0f * normalizedTime - 1.0f);

    return amplitudeAtTime;
}

float squareWave(float time, int frequency, float amplitude, float phaseShift)
{
    float period = 1.0f / frequency;
    float cycles = (time - phaseShift) / period;
    float normalizedTime = cycles - std::floor(cycles);
    float amplitudeAtTime = (normalizedTime < 0.5f) ? amplitude : -amplitude;

    return amplitudeAtTime;
}

float sineWave(float time, int frequency, float amplitude, float phaseShift)
{
    float period = 1.0f / frequency;
    float cycles = (time - phaseShift) / period;
    float angle = 2.0f * M_PI * cycles;
    float amplitudeAtTime = amplitude * std::sin(angle);

    return amplitudeAtTime;
}

float exoticWave(float time, int frequency, float amplitude, float phaseShift)
{
    float period = 1.0f / frequency;
    float cycles = (time - phaseShift) / period;
    float normalizedTime = cycles - std::floor(cycles);

    float amplitudeAtTime = 0.0f;

    if (normalizedTime <= 0.5f)
    {
        amplitudeAtTime = 2.0f * amplitude * normalizedTime;
    }
    else
    {
        amplitudeAtTime = 2.0f * amplitude * (1.0f - normalizedTime);
    }

    return amplitudeAtTime;
}

float map(float value, float fromStart, float fromEnd, float toStart, float toEnd)
{
    const float normalizedValue = (value - fromStart) / (fromEnd - fromStart);
    const float mappedValue = normalizedValue * (toEnd - toStart) + toStart;

    return mappedValue;
}
