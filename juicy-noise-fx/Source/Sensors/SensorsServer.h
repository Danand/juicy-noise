#include <iostream>
#include <queue>

#include "Sensors.h"

class SensorsServer {
public:
  SensorsServer(std::queue<Sensors> &sensorsQueue);
  void listen(uint16_t port);

private:
  std::queue<Sensors>* sensorsQueue;
};
