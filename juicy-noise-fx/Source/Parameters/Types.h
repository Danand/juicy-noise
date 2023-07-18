#pragma once

#include <JuceHeader.h>

#include "../Sensors/Sensors.h"

typedef float (*FloatFunc)(float input, float modifier);
typedef float (*SynthFunc)(float time, int frequency, float amplitude, float phaseShift);
typedef float (*SensorFunc)(const Sensors &sensors);

struct MasterParamsFloat
{
    int mapIdx;
    juce::AudioParameterFloat* valueParam;
    FloatFunc floatFunc;
};

struct SynthParams
{
    int mapIdx;
    juce::AudioParameterInt* valueMinParam;
    juce::AudioParameterInt* valueMaxParam;
    SynthFunc synthFunc;
};

struct SensorParams
{
    juce::AudioParameterInt* mapIdxParam;
    juce::AudioParameterFloat* valueMinParam;
    juce::AudioParameterFloat* valueMaxParam;
    SensorFunc sensorFunc;
};

using SynthParamFreqTuple = std::tuple<
    juce::AudioParameterInt*,
    juce::AudioParameterInt*>;

using SensorParamTuple = std::tuple<
    juce::AudioParameterFloat*,
    juce::AudioParameterFloat*,
    juce::AudioParameterInt*>;
