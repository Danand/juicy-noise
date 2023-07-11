#pragma once

#include <iostream>
#include <algorithm>
#include <cmath>

struct Sensors
{
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
    float cellSignalStrength;
    float wifiSignalStrength;

    inline Sensors operator + (const Sensors &other)
    {
        return
        {
            longitude + other.longitude,
            latitude + other.latitude,
            angularSpeedX + other.angularSpeedX,
            angularSpeedY + other.angularSpeedY,
            angularSpeedZ + other.angularSpeedZ,
            accelerationX + other.accelerationX,
            accelerationY + other.accelerationY,
            accelerationZ + other.accelerationZ,
            rotationX + other.rotationX,
            rotationY + other.rotationY,
            rotationZ + other.rotationZ,
            gravityX + other.gravityX,
            gravityY + other.gravityY,
            gravityZ + other.gravityZ,
            magneticX + other.magneticX,
            magneticY + other.magneticY,
            magneticZ + other.magneticZ,
            light + other.light,
            pressure + other.pressure,
            proximity + other.proximity,
            cellSignalStrength + other.cellSignalStrength,
            wifiSignalStrength + other.wifiSignalStrength
        };
    }

    inline Sensors operator - (const Sensors &other)
    {
        return
        {
            longitude - other.longitude,
            latitude - other.latitude,
            angularSpeedX - other.angularSpeedX,
            angularSpeedY - other.angularSpeedY,
            angularSpeedZ - other.angularSpeedZ,
            accelerationX - other.accelerationX,
            accelerationY - other.accelerationY,
            accelerationZ - other.accelerationZ,
            rotationX - other.rotationX,
            rotationY - other.rotationY,
            rotationZ - other.rotationZ,
            gravityX - other.gravityX,
            gravityY - other.gravityY,
            gravityZ - other.gravityZ,
            magneticX - other.magneticX,
            magneticY - other.magneticY,
            magneticZ - other.magneticZ,
            light - other.light,
            pressure - other.pressure,
            proximity - other.proximity,
            cellSignalStrength - other.cellSignalStrength,
            wifiSignalStrength - other.wifiSignalStrength
        };
    }

    inline Sensors operator * (const float &other)
    {
        return
        {
            longitude * other,
            latitude * other,
            angularSpeedX * other,
            angularSpeedY * other,
            angularSpeedZ * other,
            accelerationX * other,
            accelerationY * other,
            accelerationZ * other,
            rotationX * other,
            rotationY * other,
            rotationZ * other,
            gravityX * other,
            gravityY * other,
            gravityZ * other,
            magneticX * other,
            magneticY * other,
            magneticZ * other,
            light * other,
            pressure * other,
            proximity * other,
            cellSignalStrength * other,
            wifiSignalStrength * other,
        };
    }

    inline Sensors operator / (const float &other)
    {
        return
        {
            longitude / other,
            latitude / other,
            angularSpeedX / other,
            angularSpeedY / other,
            angularSpeedZ / other,
            accelerationX / other,
            accelerationY / other,
            accelerationZ / other,
            rotationX / other,
            rotationY / other,
            rotationZ / other,
            gravityX / other,
            gravityY / other,
            gravityZ / other,
            magneticX / other,
            magneticY / other,
            magneticZ / other,
            light / other,
            pressure / other,
            proximity / other,
            cellSignalStrength / other,
            wifiSignalStrength / other,
        };
    }

    inline Sensors operator * (const Sensors &other)
    {
        return
        {
            longitude * other.longitude,
            latitude * other.latitude,
            angularSpeedX * other.angularSpeedX,
            angularSpeedY * other.angularSpeedY,
            angularSpeedZ * other.angularSpeedZ,
            accelerationX * other.accelerationX,
            accelerationY * other.accelerationY,
            accelerationZ * other.accelerationZ,
            rotationX * other.rotationX,
            rotationY * other.rotationY,
            rotationZ * other.rotationZ,
            gravityX * other.gravityX,
            gravityY * other.gravityY,
            gravityZ * other.gravityZ,
            magneticX * other.magneticX,
            magneticY * other.magneticY,
            magneticZ * other.magneticZ,
            light * other.light,
            pressure * other.pressure,
            proximity * other.proximity,
            cellSignalStrength * other.cellSignalStrength,
            wifiSignalStrength * other.wifiSignalStrength
        };
    }

    inline Sensors operator / (const Sensors &other)
    {
        return
        {
            longitude / other.longitude,
            latitude / other.latitude,
            angularSpeedX / other.angularSpeedX,
            angularSpeedY / other.angularSpeedY,
            angularSpeedZ / other.angularSpeedZ,
            accelerationX / other.accelerationX,
            accelerationY / other.accelerationY,
            accelerationZ / other.accelerationZ,
            rotationX / other.rotationX,
            rotationY / other.rotationY,
            rotationZ / other.rotationZ,
            gravityX / other.gravityX,
            gravityY / other.gravityY,
            gravityZ / other.gravityZ,
            magneticX / other.magneticX,
            magneticY / other.magneticY,
            magneticZ / other.magneticZ,
            light / other.light,
            pressure / other.pressure,
            proximity / other.proximity,
            cellSignalStrength / other.cellSignalStrength,
            wifiSignalStrength / other.wifiSignalStrength
        };
    }

    inline float magnitude()
    {
        float sum = 0.0f;

        sum += longitude * longitude;
        sum += latitude * latitude;
        sum += angularSpeedX * angularSpeedX;
        sum += angularSpeedY * angularSpeedY;
        sum += angularSpeedZ * angularSpeedZ;
        sum += accelerationX * accelerationX;
        sum += accelerationY * accelerationY;
        sum += accelerationZ * accelerationZ;
        sum += rotationX * rotationX;
        sum += rotationY * rotationY;
        sum += rotationZ * rotationZ;
        sum += gravityX * gravityX;
        sum += gravityY * gravityY;
        sum += gravityZ * gravityZ;
        sum += magneticX * magneticX;
        sum += magneticY * magneticY;
        sum += magneticZ * magneticZ;
        sum += light * light;
        sum += pressure * pressure;
        sum += longitude * longitude;
        sum += proximity * proximity;
        sum += cellSignalStrength * cellSignalStrength;
        sum += wifiSignalStrength * wifiSignalStrength;

        return sqrt(sum);
    }

    float magnitudeAngularSpeed()
    {
        return sqrt(
            angularSpeedX * angularSpeedX +
            angularSpeedY * angularSpeedY +
            angularSpeedZ * angularSpeedZ);
    }

    float magnitudeAcceleration()
    {
        return sqrt(
            accelerationX * accelerationX +
            accelerationY * accelerationY +
            accelerationZ * accelerationZ);
    }

    float magnitudeRotation()
    {
        return sqrt(
            rotationX * rotationX +
            rotationY * rotationY +
            rotationZ * rotationZ);
    }

    float magnitudeGravity()
    {
        return sqrt(
            gravityX * gravityX +
            gravityY * gravityY +
            gravityZ * gravityZ);
    }

    float magnitudeMagnetic()
    {
        return sqrt(
            magneticX * magneticZ +
            magneticY * magneticY +
            magneticZ * magneticZ);
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
