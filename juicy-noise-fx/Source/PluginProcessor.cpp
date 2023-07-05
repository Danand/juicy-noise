/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <cstdlib>

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Sensors/SensorsServer.h"

class Stopwatch {
public:
    Stopwatch(std::string title) {
        this->title = title;
        this->startTime = std::chrono::high_resolution_clock::now();
    }

    ~Stopwatch() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

        std::cout << "[Stopwatch] "
                  << this->title
                  << " elapsed "
                  << duration.count()
                  << " ns."
                  << std::endl;
    }

private:
    std::chrono::_V2::system_clock::time_point startTime;
    std::string title;
};

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
        "Port",
        0,
        65535,
        6660);

    addParameter(this->portParameter);

    this->freqMinParameter = new juce::AudioParameterInt(
        "freqMinParameter",
        "Low Hz",
        20,
        22000,
        35);

    addParameter(freqMinParameter);

    this->freqMaxParameter = new juce::AudioParameterInt(
        "freqMaxParameter",
        "High Hz",
        20,
        22000,
        666);

    addParameter(this->freqMaxParameter);

    SensorsServer* sensorsServer = new SensorsServer(6660, this->sensorsQueue, mutex);
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void JuicynoisefxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
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

float calcMagnitude(float x, float y, float z) {
    return sqrt(x * x + y * y + z * z);
}

float strip_int(float value) {
    float dummy;
    return std::modf(value, &dummy);
}

int sumDigits(int number) {
    int sum = 0;

    while (number != 0) {
        int digit = number % 10;
        sum += digit;
        number /= 10;
    }

    return sum;
}

float randomFloat(float from, float to) {
    return from + static_cast<float>(rand()) / (RAND_MAX / (to - from));
}

double unclampedSin(float value) {
    const float twoPi = 2.0 * M_PI;
    value = fmod(value, twoPi);
    return std::sin(value);
}

double sawtooth(float frequency, float amplitude, float time) {
    float period = 1.0f / frequency;
    float phase = static_cast<float>(fmod(time, period)) / period;

    float value = (2.0f * phase - 1.0f) * amplitude;

    return value;
}

float map(float value, float fromStart, float fromEnd, float toStart, float toEnd) {
    float normalizedValue = (value - fromStart) / (fromEnd - fromStart);
    float mappedValue = normalizedValue * (toEnd - toStart) + toStart;

    return mappedValue;
}

void JuicynoisefxAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    mutex.lock();

    if (sensorsQueue.empty() == false) {
        this->sensorsCurrent = sensorsQueue.front();
        sensorsQueue.pop();
    }

    mutex.unlock();

    float sensorsMagnitude = this->sensorsCurrent.magnitude();

    this->sensorsMagnitudeMax = sensorsMagnitude > this->sensorsMagnitudeMax
        ? sensorsMagnitude
        : this->sensorsMagnitudeMax;

    this->sensorsMagnitudeMin = sensorsMagnitude < this->sensorsMagnitudeMin
        ? sensorsMagnitude
        : this->sensorsMagnitudeMin;

    juce::ScopedNoDenormals noDenormals;

    auto numSamples = buffer.getNumSamples();

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, numSamples);
    }

    float* channelDataLeft = buffer.getWritePointer(0);
    float* channelDataRight = buffer.getWritePointer(1);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        if (sensorsMagnitude <= 0.0001f) {
            channelDataLeft[sample] = 0.0f;
            channelDataRight[sample] = 0.0f;
            continue;
        }

        float baseValue = (channelDataLeft[sample] * 0.5f) + (channelDataRight[sample] * 0.5f);
        float baseValueDeamplified = baseValue * 0.25f;
        float time = sample / (numSamples * 1.0f);

        float frequency = map(
            sensorsMagnitude,
            this->sensorsMagnitudeMin,
            this->sensorsMagnitudeMax,
            freqMinParameter->get(),
            freqMaxParameter->get());

        float sawWave = sawtooth(frequency, 0.5f, time);
        float finalValue = baseValueDeamplified + sawWave;

        channelDataLeft[sample] = finalValue;
        channelDataRight[sample] = finalValue;
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
