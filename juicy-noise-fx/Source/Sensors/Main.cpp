#include <queue>

#include "SensorsServer.h"

int main() {
  std::queue<Sensors> sensorsQueue;
  SensorsServer sensorsServer(sensorsQueue);
  sensorsServer.listen(6660);
  return 0;
}