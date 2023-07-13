/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <iostream>
#include <atomic>

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Listeners/PortListener.h"
#include "Sensors/Sensors.h"
#include "Sensors/SensorsServer.h"
#include "Sensors/SensorsServerTypes.h"
#include "Utils/Math.h"

//==============================================================================
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

    this->amplifyParameter = new juce::AudioParameterFloat(
        "amplifyParameter",
        "amp",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->amplifyParameter);

    this->clipParameter = new juce::AudioParameterFloat(
        "clipParameter",
        "clp",
        0.0f,
        1.0f,
        1.0f);

    addParameter(this->clipParameter);

    this->freqMinSawParameter = new juce::AudioParameterInt(
        "freqMinSawParameter",
        "saw_frq_min",
        MIN_FREQ,
        MAX_FREQ,
        35);

    addParameter(this->freqMinSawParameter);

    this->freqMaxSawParameter = new juce::AudioParameterInt(
        "freqMaxSawParameter",
        "saw_frq_max",
        MIN_FREQ,
        MAX_FREQ,
        666);

    addParameter(this->freqMaxSawParameter);

    this->freqMinSquareParameter = new juce::AudioParameterInt(
        "freqMinSquareParameter",
        "sqr_frq_min",
        MIN_FREQ,
        MAX_FREQ,
        35);

    addParameter(this->freqMinSquareParameter);

    this->freqMaxSquareParameter = new juce::AudioParameterInt(
        "freqMaxSquareParameter",
        "sqr_frq_max",
        MIN_FREQ,
        MAX_FREQ,
        666);

    addParameter(this->freqMaxSquareParameter);

    this->freqMinSineParameter = new juce::AudioParameterInt(
        "freqMinSineParameter",
        "sin_frq_min",
        MIN_FREQ,
        MAX_FREQ,
        35);

    addParameter(this->freqMinSineParameter);

    this->freqMaxSineParameter = new juce::AudioParameterInt(
        "freqMaxSineParameter",
        "sin_frq_min",
        MIN_FREQ,
        MAX_FREQ,
        666);

    addParameter(this->freqMaxSineParameter);

    this->freqMinExoticParameter = new juce::AudioParameterInt(
        "freqMinExoticParameter",
        "exo_frq_min",
        MIN_FREQ,
        MAX_FREQ,
        666);

    addParameter(this->freqMinExoticParameter);

    this->freqMaxExoticParameter = new juce::AudioParameterInt(
        "freqMaxExoticParameter",
        "exo_frq_max",
        MIN_FREQ,
        MAX_FREQ,
        666);

    addParameter(this->freqMaxExoticParameter);

    this->feedbackTimeParameter = new juce::AudioParameterFloat(
        "feedbackTimeParameter",
        "fdb_time",
        0.0f,
        12.0f,
        0.0f);

    addParameter(this->feedbackTimeParameter);

    this->feedbackMixParameter = new juce::AudioParameterFloat(
        "feedbackMixParameter",
        "fdb_mix",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->feedbackMixParameter);

    this->thresholdMinLongitudeParameter = new juce::AudioParameterFloat(
        "thresholdMinLongitudeParameter",
        "lon_thr_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinLongitudeParameter);

    this->thresholdMaxLongitudeParameter = new juce::AudioParameterFloat(
        "thresholdMaxLongitudeParameter",
        "lon_thr_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxLongitudeParameter);

    this->mapLongitudeParameter = new juce::AudioParameterInt(
        "mapLongitudeParameter",
        "lon_map",
        0,
        11,
        0);

    addParameter(this->mapLongitudeParameter);

    this->thresholdMinLatitudeParameter = new juce::AudioParameterFloat(
        "thresholdMinLatitudeParameter",
        "lat_thr_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinLatitudeParameter);

    this->thresholdMaxLatitudeParameter = new juce::AudioParameterFloat(
        "thresholdMaxLatitudeParameter",
        "lat_thr_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxLatitudeParameter);

    this->mapLatitudeParameter = new juce::AudioParameterInt(
        "mapLatitudeParameter",
        "lat_map",
        0,
        11,
        0);

    addParameter(this->mapLatitudeParameter);

    this->thresholdMinAngularSpeedParameter = new juce::AudioParameterFloat(
        "thresholdMinAngularSpeedParameter",
        "ang_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinAngularSpeedParameter);

    this->thresholdMaxAngularSpeedParameter = new juce::AudioParameterFloat(
        "thresholdMaxAngularSpeedParameter",
        "ang_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxAngularSpeedParameter);

    this->mapAngularSpeedParameter = new juce::AudioParameterInt(
        "mapAngularSpeedParameter",
        "ang_map",
        0,
        11,
        0);

    addParameter(this->mapAngularSpeedParameter);

    this->thresholdMinAccelerationParameter = new juce::AudioParameterFloat(
        "thresholdMinAccelerationParameter",
        "acl_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinAccelerationParameter);

    this->thresholdMaxAccelerationParameter = new juce::AudioParameterFloat(
        "thresholdMaxAccelerationParameter",
        "acl_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxAccelerationParameter);

    this->mapAccelerationParameter = new juce::AudioParameterInt(
        "mapAccelerationParameter",
        "acl_map",
        0,
        11,
        0);

    addParameter(this->mapAccelerationParameter);

    this->thresholdMinMagneticParameter = new juce::AudioParameterFloat(
        "thresholdMinMagneticParameter",
        "mgn_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinMagneticParameter);

    this->thresholdMaxMagneticParameter = new juce::AudioParameterFloat(
        "thresholdMaxMagneticParameter",
        "mgn_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxMagneticParameter);

    this->mapMagneticParameter = new juce::AudioParameterInt(
        "mapMagneticParameter",
        "mgn_map",
        0,
        11,
        0);

    addParameter(this->mapMagneticParameter);

    this->thresholdMinLightParameter = new juce::AudioParameterFloat(
        "thresholdMinLightParameter",
        "lgt_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinLightParameter);

    this->thresholdMaxLightParameter = new juce::AudioParameterFloat(
        "thresholdMaxLightParameter",
        "lgt_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxLightParameter);

    this->mapLightParameter = new juce::AudioParameterInt(
        "mapLightParameter",
        "lgt_map",
        0,
        11,
        0);

    addParameter(this->mapLightParameter);

    this->thresholdMinPressureParameter = new juce::AudioParameterFloat(
        "thresholdMinPressureParameter",
        "prs_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinPressureParameter);

    this->thresholdMaxPressureParameter = new juce::AudioParameterFloat(
        "thresholdMaxPressureParameter",
        "prs_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxPressureParameter);

    this->mapPressureParameter = new juce::AudioParameterInt(
        "mapPressureParameter",
        "prs_map",
        0,
        11,
        0);

    addParameter(this->mapPressureParameter);

    this->thresholdMinProximityParameter = new juce::AudioParameterFloat(
        "thresholdMinProximityParameter",
        "prx_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinProximityParameter);

    this->thresholdMaxProximityParameter = new juce::AudioParameterFloat(
        "thresholdMaxProximityParameter",
        "prx_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxProximityParameter);

    this->mapProximityParameter = new juce::AudioParameterInt(
        "mapProximityParameter",
        "prx_map",
        0,
        11,
        0);

    addParameter(this->mapProximityParameter);

    this->thresholdMinCellSignalParameter = new juce::AudioParameterFloat(
        "thresholdMinCellSignalParameter",
        "cel_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinCellSignalParameter);

    this->thresholdMaxCellSignalParameter = new juce::AudioParameterFloat(
        "thresholdMaxCellSignalParameter",
        "cel_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxCellSignalParameter);

    this->mapCellSignalParameter = new juce::AudioParameterInt(
        "mapCellSignalParameter",
        "cel_ths_map",
        0,
        11,
        0);

    addParameter(this->mapCellSignalParameter);

    this->thresholdMinWifiSignalParameter = new juce::AudioParameterFloat(
        "thresholdMinWifiSignalParameter",
        "wif_ths_min",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMinWifiSignalParameter);

    this->thresholdMaxWifiSignalParameter = new juce::AudioParameterFloat(
        "thresholdMaxWifiSignalParameter",
        "wif_ths_max",
        0.0f,
        1.0f,
        0.0f);

    addParameter(this->thresholdMaxWifiSignalParameter);

    this->mapWifiSignalParameter = new juce::AudioParameterInt(
        "mapWifiSignalParameter",
        "wif_map",
        0,
        11,
        0);

    addParameter(this->mapWifiSignalParameter);

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
