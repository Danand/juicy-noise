#pragma once

#include "FixedArrays.h"

#define JN_PARAMETERS \
juce::AudioParameterInt* stereoPhaseParameter; \
 \
juce::AudioParameterFloat* amplifyParameter; \
juce::AudioParameterFloat* clipParameter; \
 \
juce::AudioParameterFloat* feedbackTimeParameter; \
juce::AudioParameterFloat* feedbackMixParameter; \
 \
juce::AudioParameterInt* freqMinSawParameter; \
juce::AudioParameterInt* freqMaxSawParameter; \
 \
juce::AudioParameterInt* freqMinSquareParameter; \
juce::AudioParameterInt* freqMaxSquareParameter; \
 \
juce::AudioParameterInt* freqMinSineParameter; \
juce::AudioParameterInt* freqMaxSineParameter; \
 \
juce::AudioParameterInt* freqMinExoticParameter; \
juce::AudioParameterInt* freqMaxExoticParameter; \
 \
juce::AudioParameterFloat* thresholdMinLongitudeParameter; \
juce::AudioParameterFloat* thresholdMaxLongitudeParameter; \
juce::AudioParameterInt* mapLongitudeParameter; \
 \
juce::AudioParameterFloat* thresholdMinLatitudeParameter; \
juce::AudioParameterFloat* thresholdMaxLatitudeParameter; \
juce::AudioParameterInt* mapLatitudeParameter; \
 \
juce::AudioParameterFloat* thresholdMinRotationXParameter; \
juce::AudioParameterFloat* thresholdMaxRotationXParameter; \
juce::AudioParameterInt* mapRotationXParameter; \
 \
juce::AudioParameterFloat* thresholdMinRotationYParameter; \
juce::AudioParameterFloat* thresholdMaxRotationYParameter; \
juce::AudioParameterInt* mapRotationYParameter; \
 \
juce::AudioParameterFloat* thresholdMinRotationZParameter; \
juce::AudioParameterFloat* thresholdMaxRotationZParameter; \
juce::AudioParameterInt* mapRotationZParameter; \
 \
juce::AudioParameterFloat* thresholdMinAngularSpeedParameter; \
juce::AudioParameterFloat* thresholdMaxAngularSpeedParameter; \
juce::AudioParameterInt* mapAngularSpeedParameter; \
 \
juce::AudioParameterFloat* thresholdMinAccelerationParameter; \
juce::AudioParameterFloat* thresholdMaxAccelerationParameter; \
juce::AudioParameterInt* mapAccelerationParameter; \
 \
juce::AudioParameterFloat* thresholdMinMagneticParameter; \
juce::AudioParameterFloat* thresholdMaxMagneticParameter; \
juce::AudioParameterInt* mapMagneticParameter; \
 \
juce::AudioParameterFloat* thresholdMinLightParameter; \
juce::AudioParameterFloat* thresholdMaxLightParameter; \
juce::AudioParameterInt* mapLightParameter; \
 \
juce::AudioParameterFloat* thresholdMinPressureParameter; \
juce::AudioParameterFloat* thresholdMaxPressureParameter; \
juce::AudioParameterInt* mapPressureParameter; \
 \
juce::AudioParameterFloat* thresholdMinProximityParameter; \
juce::AudioParameterFloat* thresholdMaxProximityParameter; \
juce::AudioParameterInt* mapProximityParameter; \
 \
juce::AudioParameterFloat* thresholdMinCellSignalParameter; \
juce::AudioParameterFloat* thresholdMaxCellSignalParameter; \
juce::AudioParameterInt* mapCellSignalParameter; \
 \
juce::AudioParameterFloat* thresholdMinWifiSignalParameter; \
juce::AudioParameterFloat* thresholdMaxWifiSignalParameter; \
juce::AudioParameterInt* mapWifiSignalParameter; \
 \
MasterParamsFloatFixed masterParamsFloat; \
SynthParamsFixed synthParams; \
SensorParamsFixed sensorsParams;
