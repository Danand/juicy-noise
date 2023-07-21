/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <iostream>
#include <atomic>
#include <tuple>

#include "PluginProcessor.h"

#include "Parameters/Assignments.h"
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

    this->addParameter(this->portParameter);

    Assignments::addSoundParameters(this, this->paramsContainer);

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
        while (sensorsQueue.empty() == false)
        {
            this->sensors = Sensors::avg(this->sensors, sensorsQueue.front());
            sensorsQueue.pop();
        }
    }
    else
    {
        Sensors sensorsEmpty;
        this->sensors = sensorsEmpty;
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
        float time = this->samplesCountInSecond / static_cast<float>(this->sampleRate);

        float inputLeft = channelDataLeft[sample];
        float inputRight = channelDataRight[sample];

        float outputLeft = inputLeft;
        float outputRight = inputRight;

        float synthOutputBlendedLeft = 0.0f;
        float synthOutputBlendedRight = 0.0f;

        for (SynthParams synthParam : this->paramsContainer.synthParams)
        {
            for (SensorParams sensorsParam : this->paramsContainer.sensorsParams)
            {
                if (sensorsParam.mapIdxParam->get() == synthParam.mapIdx)
                {
                    float sensorValueRaw = sensorsParam.sensorFunc(this->sensors);

                    float sensorValueNormalized = inverseLerp(
                        sensorsParam.valueMinParam->get(),
                        sensorsParam.valueMaxParam->get(),
                        sensorValueRaw);

                    int freq = static_cast<int>(
                        ceil(
                            lerp(
                                synthParam.valueMinParam->get(),
                                synthParam.valueMaxParam->get(),
                                sensorValueNormalized)));

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

        for (MasterParamsFloat masterParamFloat : this->paramsContainer.masterParamsFloat)
        {
            for (SensorParams sensorsParam : this->paramsContainer.sensorsParams)
            {
                if (sensorsParam.mapIdxParam->get() == masterParamFloat.mapIdx)
                {
                    float sensorValueNormalized = inverseLerp(
                        sensorsParam.valueMinParam->get(),
                        sensorsParam.valueMaxParam->get(),
                        sensorsParam.sensorFunc(this->sensors));

                    outputLeft = masterParamFloat.floatFunc(
                        outputLeft,
                        masterParamFloat.valueParam->get() * sensorValueNormalized);

                    outputRight = masterParamFloat.floatFunc(
                        outputRight,
                        masterParamFloat.valueParam->get() * sensorValueNormalized);
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
