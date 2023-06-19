#ifndef SENSORS_SERVER_H
#define SENSORS_SERVER_H

#include <iostream>
#include <queue>

#include "Sensors.h"

class SensorsServer {
public:
  void listen(uint16_t port);
  void readSensors(Sensors &sensors);

private:
  int socket;
  float buffer[sizeof(Sensors) / sizeof(float)];
};

#endif
