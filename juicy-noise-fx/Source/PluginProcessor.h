/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <queue>
#include <limits>
#include <atomic>
#include <tuple>

#include <JuceHeader.h>

#include "Parameters/ParamsContainer.h"
#include "Listeners/PortListener.h"
#include "Sensors/Sensors.h"
#include "Sensors/SensorsServer.h"
#include "Sensors/SensorsServerTypes.h"

//==============================================================================
/**
*/
class JuicynoisefxAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
                                 , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    JuicynoisefxAudioProcessor();
    ~JuicynoisefxAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:
    juce::AudioParameterInt* portParameter;
    ParamsContainer paramsContainer;
    PortListener* portListener = nullptr;
    SocketPort port;
    std::mutex sensorsMutex;
    std::queue<Sensors> sensorsQueue;
    Sensors sensors;
    int samplesCountInSecond = 0;
    float* lastBuffer = nullptr;
    std::atomic<int> latency;
    int sampleRate;
    int audioBufferSize;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuicynoisefxAudioProcessor)
};
