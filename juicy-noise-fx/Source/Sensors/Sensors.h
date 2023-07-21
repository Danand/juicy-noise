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

    inline Sensors max()
    {
        return
        {
            longitude = 1.0f,
            latitude = 1.0f,
            angularSpeedX = 1.0f,
            angularSpeedY = 1.0f,
            angularSpeedZ = 1.0f,
            accelerationX = 1.0f,
            accelerationY = 1.0f,
            accelerationZ = 1.0f,
            magneticX = 1.0f,
            magneticY = 1.0f,
            magneticZ = 1.0f,
            light = 1.0f,
            pressure = 1.0f,
            proximity = 1.0f,
            cellSignalStrength = 1.0f,
            wifiSignalStrength = 1.0f
        };
    }

    inline Sensors min()
    {
        return
        {
            longitude = -1.0f,
            latitude = -1.0f,
            angularSpeedX = -1.0f,
            angularSpeedY = -1.0f,
            angularSpeedZ = -1.0f,
            accelerationX = -1.0f,
            accelerationY = -1.0f,
            accelerationZ = -1.0f,
            magneticX = -1.0f,
            magneticY = -1.0f,
            magneticZ = -1.0f,
            light = -1.0f,
            pressure = -1.0f,
            proximity = -1.0f,
            cellSignalStrength = -1.0f,
            wifiSignalStrength = -1.0f
        };
    }

    inline Sensors operator * (const Sensors &other)
    {
        return
        {
            longitude = longitude * other.longitude,
            latitude =  latitude * other.latitude,
            angularSpeedX = angularSpeedX * other.angularSpeedX,
            angularSpeedY = angularSpeedY * other.angularSpeedY,
            angularSpeedZ = angularSpeedZ * other.angularSpeedZ,
            accelerationX = accelerationX * other.accelerationX,
            accelerationY = accelerationY * other.accelerationY,
            accelerationZ = accelerationZ * other.accelerationZ,
            magneticX = magneticX * other.magneticX,
            magneticY = magneticY * other.magneticY,
            magneticZ = magneticZ * other.magneticZ,
            light = light * other.light,
            pressure = pressure * other.pressure,
            proximity = proximity * other.proximity,
            cellSignalStrength = cellSignalStrength * other.cellSignalStrength,
            wifiSignalStrength = wifiSignalStrength * other.wifiSignalStrength
        };
    }

    inline Sensors operator / (const Sensors &other)
    {
        return
        {
            longitude = longitude / other.longitude,
            latitude =  latitude / other.latitude,
            angularSpeedX = angularSpeedX / other.angularSpeedX,
            angularSpeedY = angularSpeedY / other.angularSpeedY,
            angularSpeedZ = angularSpeedZ / other.angularSpeedZ,
            accelerationX = accelerationX / other.accelerationX,
            accelerationY = accelerationY / other.accelerationY,
            accelerationZ = accelerationZ / other.accelerationZ,
            magneticX = magneticX / other.magneticX,
            magneticY = magneticY / other.magneticY,
            magneticZ = magneticZ / other.magneticZ,
            light = light / other.light,
            pressure = pressure / other.pressure,
            proximity = proximity / other.proximity,
            cellSignalStrength = cellSignalStrength / other.cellSignalStrength,
            wifiSignalStrength = wifiSignalStrength / other.wifiSignalStrength
        };
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
