#pragma once

#include "ParamsContainer.h"

class Assignments
{
public:
    static void addSoundParameters(
        juce::AudioProcessor* processor,
        ParamsContainer &paramsContainer);

private:
    static SynthParamFreqTuple addSynthParam(
        juce::AudioProcessor* processor,
        ParamsContainer &paramsContainer,
        std::string name,
        int mapIdx,
        SynthFunc synthFunc,
        int &paramsCount);

    static juce::AudioParameterFloat* addMasterParamFloat(
        juce::AudioProcessor* processor,
        ParamsContainer &paramsContainer,
        std::string name,
        int mapIdx,
        FloatFunc floatFunc,
        int &paramsCount);

    static SensorParamTuple addSensorParam(
        juce::AudioProcessor* processor,
        ParamsContainer &paramsContainer,
        std::string name,
        float min,
        float max,
        SensorFunc sensorFunc,
        int &paramsCount);
};
