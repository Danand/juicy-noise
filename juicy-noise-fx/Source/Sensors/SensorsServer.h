#ifndef SENSORS_SERVER_H
#define SENSORS_SERVER_H

#include <iostream>
#include <queue>

#include "Sensors.h"

class SensorsServer {
public:
  void listen(uint16_t port);
  void readSensors(Sensors &sensors);
  void closeSocket();

private:
  int socket;
  char buffer[sizeof(Sensors)];
};

#endif
