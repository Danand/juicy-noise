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

juce::AudioParameterInt* JuicynoisefxAudioProcessor::AddSynthParamFreq(std::string name, int mapIdx)
{
    auto fullName = name + "_" + std::to_string(mapIdx);

    auto* parameter = new juce::AudioParameterInt(
        fullName,
        fullName,
        MIN_FREQ,
        MAX_FREQ,
        35);

    this->addParameter(parameter);

    return parameter;
}

juce::AudioParameterFloat* JuicynoisefxAudioProcessor::AddSynthParamFloat(std::string name, int mapIdx)
{
    auto fullName = name + "_" + std::to_string(mapIdx);

    auto* parameter = new juce::AudioParameterFloat(
        fullName,
        fullName,
        0.0f,
        1.0f,
        0.0f);

    this->addParameter(parameter);

    return parameter;
}

SensorParamTuple JuicynoisefxAudioProcessor::AddSensorParam(std::string name, float min, float max)
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
        0,
        11,
        0);

    this->addParameter(parameterMap);

    SensorParamTuple sensorParamTuple(parameterMin, parameterMax, parameterMap);

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

    this->amplifyParameter = AddSynthParamFloat("amp", 0);
    this->clipParameter = AddSynthParamFloat("clp", 1);

    this->freqMinSawParameter = AddSynthParamFreq("saw_frq_min", 2);
    this->freqMaxSawParameter = AddSynthParamFreq("saw_frq_max", 3);

    this->freqMinSquareParameter = AddSynthParamFreq("sqr_frq_min", 4);
    this->freqMaxSquareParameter = AddSynthParamFreq("sqr_frq_max", 5);

    this->freqMinSineParameter = AddSynthParamFreq("sin_frq_min", 6);
    this->freqMaxSineParameter = AddSynthParamFreq("sin_frq_max", 7);

    this->freqMinExoticParameter = AddSynthParamFreq("exo_frq_min", 8);
    this->freqMaxExoticParameter = AddSynthParamFreq("exo_frq_max", 9);

    this->feedbackTimeParameter = AddSynthParamFloat("fdb_time", 10);
    this->feedbackMixParameter = AddSynthParamFloat("fdb_mix", 11);

    std::tie(
        this->thresholdMinLongitudeParameter,
        this->thresholdMaxLongitudeParameter,
        this->mapLongitudeParameter) = AddSensorParam("lon", -180.0f, 180.0f);

    std::tie(
        this->thresholdMinLatitudeParameter,
        this->thresholdMaxLatitudeParameter,
        this->mapLatitudeParameter) = AddSensorParam("lat", -90.0f, 90.0f);

    std::tie(
        this->thresholdMinAngularSpeedParameter,
        this->thresholdMaxAngularSpeedParameter,
        this->mapAngularSpeedParameter) = AddSensorParam("ang", -100.0f, 100.0f);

    std::tie(
        this->thresholdMinAccelerationParameter,
        this->thresholdMaxAccelerationParameter,
        this->mapAccelerationParameter) = AddSensorParam("acl", -100.0f, 100.0f);

    std::tie(
        this->thresholdMinMagneticParameter,
        this->thresholdMaxMagneticParameter,
        this->mapMagneticParameter) = AddSensorParam("mgn", -100.0f, 100.0f);

    std::tie(
        this->thresholdMinLightParameter,
        this->thresholdMaxLightParameter,
        this->mapLightParameter) = AddSensorParam("lgt", 0.0f, 10000.0f);

    std::tie(
        this->thresholdMinPressureParameter,
        this->thresholdMaxPressureParameter,
        this->mapPressureParameter) = AddSensorParam("prs", 700.0f, 1000.0f);

    std::tie(
        this->thresholdMinProximityParameter,
        this->thresholdMaxProximityParameter,
        this->mapProximityParameter) = AddSensorParam("prx", 0.0f, 10.0f);

    std::tie(
        this->thresholdMinCellSignalParameter,
        this->thresholdMaxCellSignalParameter,
        this->mapCellSignalParameter) = AddSensorParam("cel", -100.0f, -50.0f);

    std::tie(
        this->thresholdMinWifiSignalParameter,
        this->thresholdMaxWifiSignalParameter,
        this->mapWifiSignalParameter) = AddSensorParam("wif", -100.0f, -50.0f);

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

    delete this->portListener;

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
        double time = this->samplesCountInSecond / this->sampleRate;

        float outputLeft = channelDataLeft[sample];
        float outputRight = channelDataRight[sample];

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
