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

#include <JuceHeader.h>

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
    static constexpr int MIN_FREQ = 20;
    static constexpr int MAX_FREQ = 12000;

    PortListener* portListener;
    SocketPort port;
    std::mutex sensorsMutex;
    std::queue<Sensors> sensorsQueue;
    Sensors sensors;
    int samplesCountInSecond;
    float* lastBuffer = nullptr;
    std::atomic<int> latency;
    int sampleRate;
    int audioBufferSize;

    juce::AudioParameterInt* portParameter;

    juce::AudioParameterFloat* amplifyParameter;
    juce::AudioParameterFloat* clipParameter;

    juce::AudioParameterInt* freqMinSawParameter;
    juce::AudioParameterInt* freqMaxSawParameter;

    juce::AudioParameterInt* freqMinSquareParameter;
    juce::AudioParameterInt* freqMaxSquareParameter;

    juce::AudioParameterInt* freqMinSineParameter;
    juce::AudioParameterInt* freqMaxSineParameter;

    juce::AudioParameterInt* freqMinExoticParameter;
    juce::AudioParameterInt* freqMaxExoticParameter;

    juce::AudioParameterFloat* feedbackTimeParameter;
    juce::AudioParameterFloat* feedbackMixParameter;

    juce::AudioParameterFloat* thresholdMinLongitudeParameter;
    juce::AudioParameterFloat* thresholdMaxLongitudeParameter;
    juce::AudioParameterInt* mapLongitudeParameter;

    juce::AudioParameterFloat* thresholdMinLatitudeParameter;
    juce::AudioParameterFloat* thresholdMaxLatitudeParameter;
    juce::AudioParameterInt* mapLatitudeParameter;

    juce::AudioParameterFloat* thresholdMinAngularSpeedParameter;
    juce::AudioParameterFloat* thresholdMaxAngularSpeedParameter;
    juce::AudioParameterInt* mapAngularSpeedParameter;

    juce::AudioParameterFloat* thresholdMinAccelerationParameter;
    juce::AudioParameterFloat* thresholdMaxAccelerationParameter;
    juce::AudioParameterInt* mapAccelerationParameter;

    juce::AudioParameterFloat* thresholdMinMagneticParameter;
    juce::AudioParameterFloat* thresholdMaxMagneticParameter;
    juce::AudioParameterInt* mapMagneticParameter;

    juce::AudioParameterFloat* thresholdMinLightParameter;
    juce::AudioParameterFloat* thresholdMaxLightParameter;
    juce::AudioParameterInt* mapLightParameter;

    juce::AudioParameterFloat* thresholdMinPressureParameter;
    juce::AudioParameterFloat* thresholdMaxPressureParameter;
    juce::AudioParameterInt* mapPressureParameter;

    juce::AudioParameterFloat* thresholdMinProximityParameter;
    juce::AudioParameterFloat* thresholdMaxProximityParameter;
    juce::AudioParameterInt* mapProximityParameter;

    juce::AudioParameterFloat* thresholdMinCellSignalParameter;
    juce::AudioParameterFloat* thresholdMaxCellSignalParameter;
    juce::AudioParameterInt* mapCellSignalParameter;

    juce::AudioParameterFloat* thresholdMinWifiSignalParameter;
    juce::AudioParameterFloat* thresholdMaxWifiSignalParameter;
    juce::AudioParameterInt* mapWifiSignalParameter;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuicynoisefxAudioProcessor)
};
