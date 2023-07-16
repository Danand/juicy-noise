/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <iostream>
#include <atomic>
#include <tuple>

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Listeners/PortListener.h"
#include "Sensors/Sensors.h"
#include "Sensors/SensorsServer.h"
#include "Sensors/SensorsServerTypes.h"
#include "Utils/Math.h"

//==============================================================================

SynthParamFreqTuple JuicynoisefxAudioProcessor::addSynthParam(
    SynthParamsFixed &synthParams,
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
        35);

    this->addParameter(parameterMin);

    auto fullNameMax = name + "_" + "max" + "_" + std::to_string(mapIdx);

    auto* parameterMax = new juce::AudioParameterInt(
        fullNameMax,
        fullNameMax,
        MIN_FREQ,
        MAX_FREQ,
        666);

    this->addParameter(parameterMax);

    SynthParams synthParam;

    synthParam.mapIdx = mapIdx;
    synthParam.valueMinParam = parameterMin;
    synthParam.valueMaxParam = parameterMax;
    synthParam.synthFunc = synthFunc;

    synthParams[paramsCount] = synthParam;

    paramsCount++;

    SynthParamFreqTuple synthParamTuple(parameterMin, parameterMax);

    return synthParamTuple;
}

juce::AudioParameterFloat* JuicynoisefxAudioProcessor::addMasterParamFloat(
    MasterParamsFloatFixed &masterParamsFloat,
    std::string name,
    int mapIdx,
    FloatFunc floatFunc,
    int &paramsCount)
{
    auto fullName = name + "_" + std::to_string(mapIdx);

    auto* parameter = new juce::AudioParameterFloat(
        fullName,
        fullName,
        0.0f,
        1.0f,
        0.0f);

    this->addParameter(parameter);

    MasterParamsFloat masterParam;

    masterParam.mapIdx = mapIdx;
    masterParam.valueParam = parameter;
    masterParam.floatFunc = floatFunc;

    masterParamsFloat[paramsCount] = masterParam;

    paramsCount++;

    return parameter;
}

SensorParamTuple JuicynoisefxAudioProcessor::addSensorParam(
    SensorParamsFixed &sensorParams,
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

    this->addParameter(parameterMin);

    auto fullNameMax = name + "_thr_max";

    auto* parameterMax = new juce::AudioParameterFloat(
        fullNameMax,
        fullNameMax,
        min,
        max,
        max);

    this->addParameter(parameterMax);

    auto fullNameMap = "__" + name + "_map";

    auto* parameterMap = new juce::AudioParameterInt(
        fullNameMap,
        fullNameMap,
        -1,
        7,
        -1);

    this->addParameter(parameterMap);

    SensorParams sensorsParamsItem;

    sensorsParamsItem.mapIdxParam = parameterMap;
    sensorsParamsItem.valueMinParam = parameterMin;
    sensorsParamsItem.valueMaxParam = parameterMax;
    sensorsParamsItem.sensorFunc = sensorFunc;

    sensorsParams[paramsCount] = sensorsParamsItem;

    paramsCount++;

    SensorParamTuple sensorParamTuple(
        parameterMin,
        parameterMax,
        parameterMap);

    return sensorParamTuple;
}

JuicynoisefxAudioProcessor::JuicynoisefxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
    #if ! JucePlugin_IsSynth
        .withInput("Input",  juce::AudioChannelSet::stereo(), true)
    #endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    this->portParameter = new juce::AudioParameterInt(
        "portParameter",
        "port",
        0,
        65535,
        6660);

    this->port = this->portParameter->get();
    this->portListener = new PortListener(this->port);
    this->portParameter->addListener(this->portListener);

    addParameter(this->portParameter);

    int paramsCount = 0;

    this->amplifyParameter = addMasterParamFloat(
        this->masterParamsFloat,
        "amp",
        0,
        [] (float input, float modifier) { return input + modifier; },
        paramsCount);

    this->clipParameter = addMasterParamFloat(
        this->masterParamsFloat,
        "clp",
        1,
        [] (float input, float modifier) { return std::min(input, modifier); },
        paramsCount);

        this->feedbackTimeParameter = addMasterParamFloat(
        this->masterParamsFloat,
        "fdb_time",
        2,
        [] (float input, float modifier) { return input; },
        paramsCount); // TODO: Figure out how to implement.

    this->feedbackMixParameter = addMasterParamFloat(
        this->masterParamsFloat,
        "fdb_mix",
        3,
        [] (float input, float modifier) { return input; },
        paramsCount); // TODO: Figure out how to implement.

    paramsCount = 0;

     std::tie(
        this->freqMinSawParameter,
        this->freqMaxSawParameter) = addSynthParam(
            this->synthParams,
            "saw_frq",
            4,
            sawtoothWave,
            paramsCount);

    std::tie(
        this->freqMinSquareParameter,
        this->freqMaxSquareParameter) = addSynthParam(
            this->synthParams,
            "sqr_frq",
            5,
            squareWave,
            paramsCount);

    std::tie(
        this->freqMinSineParameter,
        this->freqMaxSineParameter) = addSynthParam(
            this->synthParams,
            "sin_frq",
            6,
            sineWave,
            paramsCount);

    std::tie(
        this->freqMinExoticParameter,
        this->freqMaxExoticParameter) = addSynthParam(
            this->synthParams,
            "exo_frq",
            7,
            exoticWave,
            paramsCount);

    paramsCount = 0;

    std::tie(
        this->thresholdMinLongitudeParameter,
        this->thresholdMaxLongitudeParameter,
        this->mapLongitudeParameter) = addSensorParam(
            this->sensorsParams,
            "lon",
            -180.0f,
            180.0f,
            [] (const Sensors &sensors) { return sensors.longitude; },
            paramsCount);

    std::tie(
        this->thresholdMinLatitudeParameter,
        this->thresholdMaxLatitudeParameter,
        this->mapLatitudeParameter) = addSensorParam(
            this->sensorsParams,
            "lat",
            -90.0f,
            90.0f,
            [] (const Sensors &sensors) { return sensors.latitude; },
            paramsCount);

    std::tie(
        this->thresholdMinAngularSpeedParameter,
        this->thresholdMaxAngularSpeedParameter,
        this->mapAngularSpeedParameter) = addSensorParam(
            this->sensorsParams,
            "ang",
            -100.0f,
            100.0f,
            [] (const Sensors &sensors) { return magnitude(sensors.angularSpeedX, sensors.angularSpeedY, sensors.angularSpeedZ); },
            paramsCount);

    std::tie(
        this->thresholdMinAccelerationParameter,
        this->thresholdMaxAccelerationParameter,
        this->mapAccelerationParameter) = addSensorParam(
            this->sensorsParams,
            "acl",
            -100.0f,
            100.0f,
            [] (const Sensors &sensors) { return magnitude(sensors.accelerationX, sensors.accelerationY, sensors.accelerationZ); },
            paramsCount);

    std::tie(
        this->thresholdMinMagneticParameter,
        this->thresholdMaxMagneticParameter,
        this->mapMagneticParameter) = addSensorParam(
            this->sensorsParams,
            "mgn",
            -100.0f,
            100.0f,
            [] (const Sensors &sensors) { return magnitude(sensors.magneticX, sensors.magneticY, sensors.magneticZ); },
            paramsCount);

    std::tie(
        this->thresholdMinLightParameter,
        this->thresholdMaxLightParameter,
        this->mapLightParameter) = addSensorParam(
            this->sensorsParams,
            "lgt",
            0.0f,
            10000.0f,
            [] (const Sensors &sensors) { return sensors.light; },
            paramsCount);

    std::tie(
        this->thresholdMinPressureParameter,
        this->thresholdMaxPressureParameter,
        this->mapPressureParameter) = addSensorParam(
            this->sensorsParams,
            "prs",
            700.0f,
            1000.0f,
            [] (const Sensors &sensors) { return sensors.pressure; },
            paramsCount);

    std::tie(
        this->thresholdMinProximityParameter,
        this->thresholdMaxProximityParameter,
        this->mapProximityParameter) = addSensorParam(
            this->sensorsParams,
            "prx",
            0.0f,
            10.0f,
            [] (const Sensors &sensors) { return sensors.proximity; },
            paramsCount);

    std::tie(
        this->thresholdMinCellSignalParameter,
        this->thresholdMaxCellSignalParameter,
        this->mapCellSignalParameter) = addSensorParam(
            this->sensorsParams,
            "cel",
            -100.0f,
            -50.0f,
            [] (const Sensors &sensors) { return sensors.cellSignalStrength; },
            paramsCount);

    std::tie(
        this->thresholdMinWifiSignalParameter,
        this->thresholdMaxWifiSignalParameter,
        this->mapWifiSignalParameter) = addSensorParam(
            this->sensorsParams,
            "wif",
            -100.0f,
            -50.0f,
            [] (const Sensors &sensors) { return sensors.wifiSignalStrength; },
            paramsCount);

    SensorsServer* sensorsServer = new SensorsServer(
        this->latency,
        this->port,
        this->sensorsQueue,
        sensorsMutex);
}

JuicynoisefxAudioProcessor::~JuicynoisefxAudioProcessor()
{
}

//==============================================================================
const juce::String JuicynoisefxAudioProcessor::getName() const
{
    return "JuicyNoiseFX";
}

bool JuicynoisefxAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool JuicynoisefxAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool JuicynoisefxAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double JuicynoisefxAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JuicynoisefxAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JuicynoisefxAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JuicynoisefxAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String JuicynoisefxAudioProcessor::getProgramName(int index)
{
    return {};
}

void JuicynoisefxAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void JuicynoisefxAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = static_cast<int>(floor(sampleRate));
    this->audioBufferSize = samplesPerBlock;

    int latency = static_cast<int>(floor(samplesPerBlock / (sampleRate / 1000.0)));

    this->latency.store(latency);

    if (this->lastBuffer != nullptr)
    {
        delete[] this->lastBuffer;
        this->lastBuffer = nullptr;
    }

    this->lastBuffer = new float[samplesPerBlock];

    std::fill(
        this->lastBuffer,
        this->lastBuffer + samplesPerBlock,
        0.0f);
}

void JuicynoisefxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    this->portParameter->removeListener(this->portListener);

    if (this->portListener != nullptr)
    {
        delete this->portListener;
    }

    if (this->lastBuffer != nullptr)
    {
        delete[] this->lastBuffer;
        this->lastBuffer = nullptr;
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JuicynoisefxAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
  #endif
}
#endif

void JuicynoisefxAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    sensorsMutex.lock();

    if (sensorsQueue.empty() == false)
    {
        this->sensors = sensorsQueue.front();
        sensorsQueue.pop();
    }

    sensorsMutex.unlock();

    juce::ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();

    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, numSamples);
    }

    float* channelDataLeft = buffer.getWritePointer(0);
    float* channelDataRight = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        double time = this->samplesCountInSecond / static_cast<float>(this->sampleRate);

        float inputLeft = channelDataLeft[sample];
        float inputRight = channelDataRight[sample];

        float outputLeft = inputLeft;
        float outputRight = inputRight;

        float synthOutputBlendedLeft = 0.0f;
        float synthOutputBlendedRight = 0.0f;

        for (SynthParams synthParam : this->synthParams)
        {
            for (SensorParams sensorsParam : this->sensorsParams)
            {
                if (sensorsParam.mapIdxParam->get() == synthParam.mapIdx)
                {
                    float normalizedSensorValue = inverseLerp(
                        sensorsParam.valueMinParam->get(),
                        sensorsParam.valueMaxParam->get(),
                        sensorsParam.sensorFunc(this->sensors));

                    int freq = static_cast<int>(
                        ceil(
                            lerp(
                                synthParam.valueMinParam->get(),
                                synthParam.valueMaxParam->get(),
                                normalizedSensorValue)));

                    float phaseLeft = 0.0f;

                    float synthOutputLeft = synthParam.synthFunc(
                        time,
                        freq,
                        0.85f,
                        phaseLeft);

                    synthOutputBlendedLeft = std::max(synthOutputBlendedLeft, synthOutputLeft);

                    float phaseRight = 0.001f;

                    float synthOutputRight = synthParam.synthFunc(
                        time,
                        freq,
                        0.85f,
                        phaseRight);

                    synthOutputBlendedRight = std::max(synthOutputBlendedRight, synthOutputRight);
                }
            }
        }

        outputLeft = synthOutputBlendedLeft;
        outputRight = synthOutputBlendedRight;

        for (MasterParamsFloat masterParamFloat : this->masterParamsFloat)
        {
            for (SensorParams sensorsParam : this->sensorsParams)
            {
                if (sensorsParam.mapIdxParam->get() == masterParamFloat.mapIdx)
                {
                    float normalizedSensorValue = inverseLerp(
                        sensorsParam.valueMinParam->get(),
                        sensorsParam.valueMaxParam->get(),
                        sensorsParam.sensorFunc(this->sensors));

                    outputLeft = masterParamFloat.floatFunc(outputLeft, masterParamFloat.valueParam->get() * normalizedSensorValue);
                    outputRight = masterParamFloat.floatFunc(outputRight, masterParamFloat.valueParam->get() * normalizedSensorValue);
                }
            }
        }

        channelDataLeft[sample] = outputLeft;
        channelDataRight[sample] = outputRight;

        this->lastBuffer[sample] = outputLeft;

        this->samplesCountInSecond++;

        if (this->samplesCountInSecond > this->sampleRate)
        {
            this->samplesCountInSecond = 0;
        }
    }
}

//==============================================================================
bool JuicynoisefxAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JuicynoisefxAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void JuicynoisefxAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JuicynoisefxAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JuicynoisefxAudioProcessor();
}
