#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>

struct Sensors
{
    float longitude = 0.0f;
    float latitude = 0.0f;
    float angularSpeedX = 0.0f;
    float angularSpeedY = 0.0f;
    float angularSpeedZ = 0.0f;
    float accelerationX = 0.0f;
    float accelerationY = 0.0f;
    float accelerationZ = 0.0f;
    float rotationX = 0.0f;
    float rotationY = 0.0f;
    float rotationZ = 0.0f;
    float gravityX = 0.0f;
    float gravityY = 0.0f;
    float gravityZ = 0.0f;
    float magneticX = 0.0f;
    float magneticY = 0.0f;
    float magneticZ = 0.0f;
    float light = 0.0f;
    float pressure = 0.0f;
    float proximity = 0.0f;
    float cellSignalStrength = 0.0f;
    float wifiSignalStrength = 0.0f;

    static inline Sensors avg(const Sensors &left, const Sensors &right)
    {
        Sensors result;

        result.longitude = (left.longitude + right.longitude) / 2.0f;
        result.latitude = (left.latitude + right.latitude) / 2.0f;
        result.angularSpeedX = (left.angularSpeedX + right.angularSpeedX) / 2.0f;
        result.angularSpeedY = (left.angularSpeedY + right.angularSpeedY) / 2.0f;
        result.angularSpeedZ = (left.angularSpeedZ + right.angularSpeedZ) / 2.0f;
        result.accelerationX = (left.accelerationX + right.accelerationX) / 2.0f;
        result.accelerationY = (left.accelerationY + right.accelerationY) / 2.0f;
        result.accelerationZ = (left.accelerationZ + right.accelerationZ) / 2.0f;
        result.magneticX = (left.magneticX + right.magneticX) / 2.0f;
        result.magneticY = (left.magneticY + right.magneticY) / 2.0f;
        result.magneticZ = (left.magneticZ + right.magneticZ) / 2.0f;
        result.light = (left.light + right.light) / 2.0f;
        result.pressure = (left.pressure + right.pressure) / 2.0f;
        result.proximity = (left.proximity + right.proximity) / 2.0f;
        result.cellSignalStrength = (left.cellSignalStrength + right.cellSignalStrength) / 2.0f;
        result.wifiSignalStrength = (left.wifiSignalStrength + right.wifiSignalStrength) / 2.0f;

        return result;
    }
};

constexpr int SIZEOF_SENSORS = sizeof(Sensors);

inline std::ostream& operator << (std::ostream &stream, Sensors &sensors)
{
    stream << "{"    << "\n";
    stream << "    " << "\"longitude\": "          << sensors.longitude          << "," << "\n";
    stream << "    " << "\"latitude\": "           << sensors.latitude           << "," << "\n";
    stream << "    " << "\"angularSpeedX\": "      << sensors.angularSpeedX      << "," << "\n";
    stream << "    " << "\"angularSpeedY\": "      << sensors.angularSpeedY      << "," << "\n";
    stream << "    " << "\"angularSpeedZ\": "      << sensors.angularSpeedZ      << "," << "\n";
    stream << "    " << "\"accelerationX\": "      << sensors.accelerationX      << "," << "\n";
    stream << "    " << "\"accelerationY\": "      << sensors.accelerationY      << "," << "\n";
    stream << "    " << "\"accelerationZ\": "      << sensors.accelerationZ      << "," << "\n";
    stream << "    " << "\"rotationX\": "          << sensors.rotationX          << "," << "\n";
    stream << "    " << "\"rotationY\": "          << sensors.rotationY          << "," << "\n";
    stream << "    " << "\"rotationZ\": "          << sensors.rotationZ          << "," << "\n";
    stream << "    " << "\"gravityX\": "           << sensors.gravityX           << "," << "\n";
    stream << "    " << "\"gravityY\": "           << sensors.gravityY           << "," << "\n";
    stream << "    " << "\"gravityZ\": "           << sensors.gravityZ           << "," << "\n";
    stream << "    " << "\"magneticX\": "          << sensors.magneticX          << "," << "\n";
    stream << "    " << "\"magneticY\": "          << sensors.magneticY          << "," << "\n";
    stream << "    " << "\"magneticZ\": "          << sensors.magneticZ          << "," << "\n";
    stream << "    " << "\"light\": "              << sensors.light              << "," << "\n";
    stream << "    " << "\"pressure\": "           << sensors.pressure           << "," << "\n";
    stream << "    " << "\"proximity\": "          << sensors.proximity          << "," << "\n";
    stream << "    " << "\"cellSignalStrength\": " << sensors.cellSignalStrength << "," << "\n";
    stream << "    " << "\"wifiSignalStrength\": " << sensors.wifiSignalStrength << "\n";
    stream << "}";

    return stream;
}
