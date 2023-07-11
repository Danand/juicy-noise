#pragma once

#include <queue>

#include "Sensors.h"

using SensorsQueue = std::queue<Sensors>;
using SensorsBuffer = char[SIZEOF_SENSORS];

typedef int SocketDescriptor;
typedef uint16_t SocketPort;
