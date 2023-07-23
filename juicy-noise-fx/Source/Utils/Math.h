#pragma once

#include <cmath>
#include <random>

using SwapSignalTuple = std::tuple<
    bool,
    float>;

float magnitude(float x, float y, float z)
{
    return sqrt(
        (x * x) +
        (y * y) +
        (z * z));
}

SwapSignalTuple resolveBlend(float previous, float current, float threshold)
{
    if (threshold - current < 0.01f)
    {
        return SwapSignalTuple(
            true,
            current);
    }
    else
    {
        return SwapSignalTuple(
            false,
            std::max(
                previous,
                current));
    }
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

float map(float value, float fromStart, float fromEnd, float toStart, float toEnd)
{
    const float normalizedValue = (value - fromStart) / (fromEnd - fromStart);
    const float mappedValue = normalizedValue * (toEnd - toStart) + toStart;

    return mappedValue;
}
