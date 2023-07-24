#include "Assignments.h"
#include "../Constants/Freq.h"
#include "../Utils/Synths.h"

juce::AudioParameterInt* Assignments::addParamInt(
    juce::AudioProcessor* processor,
    std::string name,
    int min,
    int max)
{
    juce::AudioParameterInt* parameter = new juce::AudioParameterInt(
        name,
        name,
        min,
        max,
        min);

    processor->addParameter(parameter);

    return parameter;
}

juce::AudioParameterFloat* Assignments::addParamFloat(
    juce::AudioProcessor* processor,
    std::string name,
    float min,
    float max)
{
    juce::AudioParameterFloat* parameter = new juce::AudioParameterFloat(
        name,
        name,
        min,
        max,
        min);

    processor->addParameter(parameter);

    return parameter;
}

SynthParamFreqTuple Assignments::addSynthParam(
    juce::AudioProcessor* processor,
    ParamsContainer &paramsContainer,
    std::string name,
    int mapIdx,
    SynthFunc synthFunc,
    int &paramsCount)
{
    auto fullNameMin = name + "_" + "min" + "_" + std::to_string(mapIdx);

    auto* parameterMin = new juce::AudioParameterInt(
        fullNameMin,
        fullNameMin,
        MIN_FREQ,
        MAX_FREQ,
        DEFAULT_LOW_FREQ);

    processor->addParameter(parameterMin);

    auto fullNameMax = name + "_" + "max" + "_" + std::to_string(mapIdx);

    auto* parameterMax = new juce::AudioParameterInt(
        fullNameMax,
        fullNameMax,
        MIN_FREQ,
        MAX_FREQ,
        DEFAULT_HIGH_FREQ);

    processor->addParameter(parameterMax);

    SynthParams synthParam;

    synthParam.mapIdx = mapIdx;
    synthParam.valueMinParam = parameterMin;
    synthParam.valueMaxParam = parameterMax;
    synthParam.synthFunc = synthFunc;

    paramsContainer.synthParams[paramsCount] = synthParam;

    paramsCount++;

    SynthParamFreqTuple synthParamTuple(parameterMin, parameterMax);

    return synthParamTuple;
}

juce::AudioParameterFloat* Assignments::addMasterParamFloat(
    juce::AudioProcessor* processor,
    ParamsContainer &paramsContainer,
    std::string name,
    int mapIdx,
    FloatFunc floatFunc,
    int &paramsCount,
    float defaultValue,
    float minValue,
    float maxValue)
{
    auto fullName = name + "_" + std::to_string(mapIdx);

    auto* parameter = new juce::AudioParameterFloat(
        fullName,
        fullName,
        minValue,
        maxValue,
        defaultValue);

    processor->addParameter(parameter);

    MasterParamsFloat masterParam;

    masterParam.mapIdx = mapIdx;
    masterParam.valueParam = parameter;
    masterParam.floatFunc = floatFunc;

    paramsContainer.masterParamsFloat[paramsCount] = masterParam;

    paramsCount++;

    return parameter;
}

SensorParamTuple Assignments::addSensorParam(
    juce::AudioProcessor* processor,
    ParamsContainer &paramsContainer,
    std::string name,
    float min,
    float max,
    SensorFunc sensorFunc,
    int &paramsCount)
{
    auto fullNameMin = name + "_thr_min";

    auto* parameterMin = new juce::AudioParameterFloat(
        fullNameMin,
        fullNameMin,
        min,
        max,
        min);

    processor->addParameter(parameterMin);

    auto fullNameMax = name + "_thr_max";

    auto* parameterMax = new juce::AudioParameterFloat(
        fullNameMax,
        fullNameMax,
        min,
        max,
        max);

    processor->addParameter(parameterMax);

    auto fullNameMap = "__" + name + "_map";

    auto* parameterMap = new juce::AudioParameterInt(
        fullNameMap,
        fullNameMap,
        -1,
        7,
        -1);

    processor->addParameter(parameterMap);

    SensorParams sensorParam;

    sensorParam.mapIdxParam = parameterMap;
    sensorParam.valueMinParam = parameterMin;
    sensorParam.valueMaxParam = parameterMax;
    sensorParam.sensorFunc = sensorFunc;

    paramsContainer.sensorsParams[paramsCount] = sensorParam;

    paramsCount++;

    SensorParamTuple sensorParamTuple(
        parameterMin,
        parameterMax,
        parameterMap);

    return sensorParamTuple;
}

float amplify(
    float input,
    float paramValue,
    float sensorValueNormalized,
    MasterContext &masterContext)
{
    return input * (1.0f + (paramValue * sensorValueNormalized));
}

float clip(
    float input,
    float paramValue,
    float sensorValueNormalized,
    MasterContext &masterContext)
{
    float delta = paramValue - input;

    if (delta >= 0.0f)
    {
        return input;
    }
    else
    {
        return input - (delta * sensorValueNormalized);
    }
}

float setFeedbackTime(
    float input,
    float paramValue,
    float sensorValueNormalized,
    MasterContext &masterContext)
{
    masterContext.feedbackTime = paramValue * sensorValueNormalized;
    return input;
}

float getSampleAtTimeAgo(const MasterContext &masterContext, float timeAgo)
{
    float iteratedTime = 0.0f;

    for (int bufferIdx = masterContext.lastBuffers.size() - 1; bufferIdx >= 0; --bufferIdx)
    {
        SampleBuffer buffer = masterContext.lastBuffers[bufferIdx];

        int bufferSize = buffer.size();

        for (int sampleIdx = 0; sampleIdx < bufferSize; ++sampleIdx)
        {
            iteratedTime += 1.0f / (masterContext.sampleRate / static_cast<float>(bufferSize));

            if (iteratedTime >= timeAgo)
            {
                return buffer[sampleIdx];
            }
        }
    }

    return 0;
}

float feedback(
    float input,
    float paramValue,
    float sensorValueNormalized,
    MasterContext &masterContext)
{
    if (masterContext.feedbackTime < 0.001f)
    {
        return input;
    }

    if (masterContext.feedbackTimeRemaining < 0.001f)
    {
        masterContext.feedbackTimeRemaining = masterContext.feedbackTime;
    }

    float progress = 1.0f - (masterContext.feedbackTimeRemaining / masterContext.feedbackTime);

    float mix = paramValue * sensorValueNormalized;

    float feedbackRatio = lerpSynth(0, mix, progress);

    float bufferedValue = getSampleAtTimeAgo(
        masterContext,
        masterContext.feedbackTime * progress);

    int bufferSize = masterContext.lastBuffers.back().size();

    masterContext.feedbackTimeRemaining -= 1.0f / (masterContext.sampleRate / static_cast<float>(bufferSize));

    return input + (bufferedValue * feedbackRatio);
}

void Assignments::addSoundParameters(
    juce::AudioProcessor* processor,
    ParamsContainer &paramsContainer)
{
    // Parameters are non-mapped to sensors:

    paramsContainer.stereoPhaseParameter = addParamInt(
        processor,
        "str_phs",
        0,
        1000);

    paramsContainer.inputBlendParameter = addParamFloat(
        processor,
        "ipt_bln",
        0.0f,
        1.0f);

    // Parameters are mapped to sensors:

    int paramsCount = 0;

    paramsContainer.amplifyParameter = addMasterParamFloat(
        processor,
        paramsContainer,
        "amp",
        0,
        amplify,
        paramsCount);

    paramsContainer.clipParameter = addMasterParamFloat(
        processor,
        paramsContainer,
        "clp",
        1,
        clip,
        paramsCount,
        1.0f);

    paramsContainer.feedbackTimeParameter = addMasterParamFloat(
        processor,
        paramsContainer,
        "fdb_time",
        2,
        setFeedbackTime,
        paramsCount,
        0.0f,
        0.0f,
        60.0f);

    paramsContainer.feedbackMixParameter = addMasterParamFloat(
        processor,
        paramsContainer,
        "fdb_mix",
        3,
        feedback,
        paramsCount);

    paramsCount = 0;

    std::tie(
        paramsContainer.freqMinSawParameter,
        paramsContainer.freqMaxSawParameter) = addSynthParam(
            processor,
            paramsContainer,
            "saw_frq",
            4,
            sawtoothWave,
            paramsCount);

    std::tie(
        paramsContainer.freqMinSquareParameter,
        paramsContainer.freqMaxSquareParameter) = addSynthParam(
            processor,
            paramsContainer,
            "sqr_frq",
            5,
            squareWave,
            paramsCount);

    std::tie(
        paramsContainer.freqMinSineParameter,
        paramsContainer.freqMaxSineParameter) = addSynthParam(
            processor,
            paramsContainer,
            "sin_frq",
            6,
            sineWave,
            paramsCount);

    std::tie(
        paramsContainer.freqMinExoticParameter,
        paramsContainer.freqMaxExoticParameter) = addSynthParam(
            processor,
            paramsContainer,
            "exo_frq",
            7,
            exoticWave,
            paramsCount);

    paramsCount = 0;

    std::tie(
        paramsContainer.thresholdMinLongitudeParameter,
        paramsContainer.thresholdMaxLongitudeParameter,
        paramsContainer.mapLongitudeParameter) = addSensorParam(
            processor,
            paramsContainer,
            "lon",
            -180.0f,
            180.0f,
            [] (const Sensors &sensors) { return sensors.longitude; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinLatitudeParameter,
        paramsContainer.thresholdMaxLatitudeParameter,
        paramsContainer.mapLatitudeParameter) = addSensorParam(
            processor,
            paramsContainer,
            "lat",
            -90.0f,
            90.0f,
            [] (const Sensors &sensors) { return sensors.latitude; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMaxRotationXParameter,
        paramsContainer.thresholdMinRotationXParameter,
        paramsContainer.mapRotationXParameter) = addSensorParam(
            processor,
            paramsContainer,
            "rot_x",
            -1.0f,
            1.0f,
            [] (const Sensors &sensors) { return sensors.rotationX; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMaxRotationYParameter,
        paramsContainer.thresholdMinRotationYParameter,
        paramsContainer.mapRotationYParameter) = addSensorParam(
            processor,
            paramsContainer,
            "rot_y",
            -1.0f,
            1.0f,
            [] (const Sensors &sensors) { return sensors.rotationY; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMaxRotationZParameter,
        paramsContainer.thresholdMinRotationZParameter,
        paramsContainer.mapRotationZParameter) = addSensorParam(
            processor,
            paramsContainer,
            "rot_z",
            -1.0f,
            1.0f,
            [] (const Sensors &sensors) { return sensors.rotationZ; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinAngularSpeedParameter,
        paramsContainer.thresholdMaxAngularSpeedParameter,
        paramsContainer.mapAngularSpeedParameter) = addSensorParam(
            processor,
            paramsContainer,
            "ang",
            0.0f,
            1.0f,
            [] (const Sensors &sensors) { return magnitudeSynth(sensors.angularSpeedX, sensors.angularSpeedY, sensors.angularSpeedZ); },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinAccelerationParameter,
        paramsContainer.thresholdMaxAccelerationParameter,
        paramsContainer.mapAccelerationParameter) = addSensorParam(
            processor,
            paramsContainer,
            "acl",
            0.0f,
            10.0f,
            [] (const Sensors &sensors) { return magnitudeSynth(sensors.accelerationX, sensors.accelerationY, sensors.accelerationZ); },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinMagneticParameter,
        paramsContainer.thresholdMaxMagneticParameter,
        paramsContainer.mapMagneticParameter) = addSensorParam(
            processor,
            paramsContainer,
            "mgn",
            0.0f,
            200.0f,
            [] (const Sensors &sensors) { return magnitudeSynth(sensors.magneticX, sensors.magneticY, sensors.magneticZ); },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinLightParameter,
        paramsContainer.thresholdMaxLightParameter,
        paramsContainer.mapLightParameter) = addSensorParam(
            processor,
            paramsContainer,
            "lgt",
            0.0f,
            100.0f,
            [] (const Sensors &sensors) { return sensors.light; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinPressureParameter,
        paramsContainer.thresholdMaxPressureParameter,
        paramsContainer.mapPressureParameter) = addSensorParam(
            processor,
            paramsContainer,
            "prs",
            900.0f,
            1000.0f,
            [] (const Sensors &sensors) { return sensors.pressure; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinProximityParameter,
        paramsContainer.thresholdMaxProximityParameter,
        paramsContainer.mapProximityParameter) = addSensorParam(
            processor,
            paramsContainer,
            "prx",
            0.0f,
            10.0f,
            [] (const Sensors &sensors) { return sensors.proximity; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinCellSignalParameter,
        paramsContainer.thresholdMaxCellSignalParameter,
        paramsContainer.mapCellSignalParameter) = addSensorParam(
            processor,
            paramsContainer,
            "cel",
            -100.0f,
            -50.0f,
            [] (const Sensors &sensors) { return sensors.cellSignalStrength; },
            paramsCount);

    std::tie(
        paramsContainer.thresholdMinWifiSignalParameter,
        paramsContainer.thresholdMaxWifiSignalParameter,
        paramsContainer.mapWifiSignalParameter) = addSensorParam(
            processor,
            paramsContainer,
            "wif",
            -100.0f,
            -50.0f,
            [] (const Sensors &sensors) { return sensors.wifiSignalStrength; },
            paramsCount);
}
