#pragma once

#include <vector>

#include <JuceHeader.h>

#include "../Sensors/Sensors.h"

using SampleBuffer = std::vector<float>;
using SampleBuffers = std::vector<SampleBuffer>;

struct MasterContext
{
    float feedbackTime;
    float feedbackTimeRemaining;
    int sampleRate;
    SampleBuffers lastBuffers;
};

typedef float (*FloatFunc)(float input, float paramValue, float sensorValueNormalized, MasterContext &masterContext);
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
