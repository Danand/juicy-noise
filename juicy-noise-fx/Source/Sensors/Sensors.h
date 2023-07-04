#pragma once

#include <iostream>

struct Sensors {
    float longitude;
    float latitude;
    float angularSpeedX;
    float angularSpeedY;
    float angularSpeedZ;
    float accelerationX;
    float accelerationY;
    float accelerationZ;
    float rotationX;
    float rotationY;
    float rotationZ;
    float gravityX;
    float gravityY;
    float gravityZ;
    float magneticX;
    float magneticY;
    float magneticZ;
    float light;
    float pressure;
    float proximity;
};

constexpr int SIZEOF_SENSORS = sizeof(Sensors);

inline std::ostream& operator <<(std::ostream& stream, const Sensors& sensors) {
    stream << "{"    << "\n";
    stream << "    " << "\"longitude\": "     << sensors.longitude     << "," << "\n";
    stream << "    " << "\"latitude\": "      << sensors.latitude      << "," << "\n";
    stream << "    " << "\"angularSpeedX\": " << sensors.angularSpeedX << "," << "\n";
    stream << "    " << "\"angularSpeedY\": " << sensors.angularSpeedY << "," << "\n";
    stream << "    " << "\"angularSpeedZ\": " << sensors.angularSpeedZ << "," << "\n";
    stream << "    " << "\"accelerationX\": " << sensors.accelerationX << "," << "\n";
    stream << "    " << "\"accelerationY\": " << sensors.accelerationY << "," << "\n";
    stream << "    " << "\"accelerationZ\": " << sensors.accelerationZ << "," << "\n";
    stream << "    " << "\"rotationX\": "     << sensors.rotationX     << "," << "\n";
    stream << "    " << "\"rotationY\": "     << sensors.rotationY     << "," << "\n";
    stream << "    " << "\"rotationZ\": "     << sensors.rotationZ     << "," << "\n";
    stream << "    " << "\"gravityX\": "      << sensors.gravityX      << "," << "\n";
    stream << "    " << "\"gravityY\": "      << sensors.gravityY      << "," << "\n";
    stream << "    " << "\"gravityZ\": "      << sensors.gravityZ      << "," << "\n";
    stream << "    " << "\"magneticX\": "     << sensors.magneticX     << "," << "\n";
    stream << "    " << "\"magneticY\": "     << sensors.magneticY     << "," << "\n";
    stream << "    " << "\"magneticZ\": "     << sensors.magneticZ     << "," << "\n";
    stream << "    " << "\"light\": "         << sensors.light         << "," << "\n";
    stream << "    " << "\"pressure\": "      << sensors.pressure      << "," << "\n";
    stream << "    " << "\"proximity\": "     << sensors.proximity     << "\n";
    stream << "}";

    return stream;
}
