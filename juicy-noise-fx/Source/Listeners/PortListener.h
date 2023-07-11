#pragma once

#include <JuceHeader.h>

#include "../Sensors/SensorsServerTypes.h"

class PortListener : public juce::AudioProcessorParameter::Listener
{
public:
    PortListener(SocketPort &port) : port(port) { }

    void parameterValueChanged(int parameterIndex, float newValue) override
    {
        port = static_cast<SocketPort>(newValue);
    }

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override
    {
    }

private:
    SocketPort &port;
};
