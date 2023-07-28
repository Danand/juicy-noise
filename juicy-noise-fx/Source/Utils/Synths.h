#pragma once

#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// TODO: Remove duplicate between this file and `Math.h`:
float magnitudeSynth(float x, float y, float z)
{
    return sqrt(
        (x * x) +
        (y * y) +
        (z * z));
}

float lerpSynth(float from, float to, float ratio)
{
    return std::abs((1.0f - ratio) * from + (to * ratio));
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
