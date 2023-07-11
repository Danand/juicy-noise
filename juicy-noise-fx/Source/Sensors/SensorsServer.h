#pragma once

#include <iostream>
#include <queue>
#include <atomic>

#include <JuceHeader.h>

#include "Sensors.h"

using SensorsQueue = std::queue<Sensors>;
using SensorsBuffer = char[SIZEOF_SENSORS];

typedef int SocketDescriptor;
typedef uint16_t SocketPort;

constexpr int THREAD_STOP_DURATION = 1000;
constexpr SocketDescriptor EMPTY_SOCKET_DESCRIPTOR = -1;

class SensorsServer : public juce::Thread
{
public:
    SensorsServer(
        std::atomic<int>& latency,
        SocketPort port,
        SensorsQueue& sensorsQueue,
        std::mutex& mutex)
            : juce::Thread("SensorsServer"),
              latency(latency),
              port(port),
              sensorsQueue(sensorsQueue),
              mutex(mutex)
    {
        startThread();
    }

    void stop();

    ~SensorsServer() override
    {
        stop();
        stopThread(THREAD_STOP_DURATION);
    }

    void run() override;

private:
    SocketPort port;
    std::mutex& mutex;
    SensorsQueue& sensorsQueue;
    SocketDescriptor serverSocketDescriptor;
    SocketDescriptor clientSocketDescriptor;
    SensorsBuffer sensorsBuffer;
    Sensors sensors;
    std::atomic<int>& latency;
};
