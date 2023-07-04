#pragma once

#include <iostream>
#include <queue>

#include <JuceHeader.h>

#include "Sensors.h"

using SensorsQueue = std::queue<Sensors>;
using SensorsBuffer = char[SIZEOF_SENSORS];

typedef int SocketDescriptor;
typedef uint16_t SocketPort;

constexpr int THREAD_STOP_DURATION = 1000;
constexpr int THREAD_DELAY = 100;
constexpr SocketDescriptor EMPTY_SOCKET_DESCRIPTOR = -1;

class SensorsServer : public juce::Thread {

public:
  SensorsServer(SocketPort port, SensorsQueue& sensorsQueue, std::mutex& mutex)
    : juce::Thread("SensorsServer"),
      port(port),
      sensorsQueue(sensorsQueue),
      mutex(mutex) {
    startThread();
  }

  void stop();

  ~SensorsServer() override {
    stop();
    stopThread(THREAD_STOP_DURATION);
  }

  void run() override;

private:
  SocketPort port;
  std::mutex& mutex;
  SensorsQueue& sensorsQueue;
  SocketDescriptor server_socket_descriptor;
  SocketDescriptor client_socket_descriptor;
  SensorsBuffer sensorsBuffer;
  Sensors sensors;
};
