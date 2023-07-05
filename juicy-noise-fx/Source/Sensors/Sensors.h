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

    inline std::ostream& operator <<(std::ostream& stream) {
        stream << "{"    << "\n";
        stream << "    " << "\"longitude\": "     << longitude     << "," << "\n";
        stream << "    " << "\"latitude\": "      << latitude      << "," << "\n";
        stream << "    " << "\"angularSpeedX\": " << angularSpeedX << "," << "\n";
        stream << "    " << "\"angularSpeedY\": " << angularSpeedY << "," << "\n";
        stream << "    " << "\"angularSpeedZ\": " << angularSpeedZ << "," << "\n";
        stream << "    " << "\"accelerationX\": " << accelerationX << "," << "\n";
        stream << "    " << "\"accelerationY\": " << accelerationY << "," << "\n";
        stream << "    " << "\"accelerationZ\": " << accelerationZ << "," << "\n";
        stream << "    " << "\"rotationX\": "     << rotationX     << "," << "\n";
        stream << "    " << "\"rotationY\": "     << rotationY     << "," << "\n";
        stream << "    " << "\"rotationZ\": "     << rotationZ     << "," << "\n";
        stream << "    " << "\"gravityX\": "      << gravityX      << "," << "\n";
        stream << "    " << "\"gravityY\": "      << gravityY      << "," << "\n";
        stream << "    " << "\"gravityZ\": "      << gravityZ      << "," << "\n";
        stream << "    " << "\"magneticX\": "     << magneticX     << "," << "\n";
        stream << "    " << "\"magneticY\": "     << magneticY     << "," << "\n";
        stream << "    " << "\"magneticZ\": "     << magneticZ     << "," << "\n";
        stream << "    " << "\"light\": "         << light         << "," << "\n";
        stream << "    " << "\"pressure\": "      << pressure      << "," << "\n";
        stream << "    " << "\"proximity\": "     << proximity     << "\n";
        stream << "}";

        return stream;
    }

    inline Sensors operator - (const Sensors& right) {
        return {
            longitude - right.longitude,
            latitude - right.latitude,
            angularSpeedX - right.angularSpeedX,
            angularSpeedY - right.angularSpeedY,
            angularSpeedZ - right.angularSpeedZ,
            accelerationX - right.accelerationX,
            accelerationY - right.accelerationY,
            accelerationZ - right.accelerationZ,
            rotationX - right.rotationX,
            rotationY - right.rotationY,
            rotationZ - right.rotationZ,
            gravityX - right.gravityX,
            gravityY - right.gravityY,
            gravityZ - right.gravityZ,
            magneticX - right.magneticX,
            magneticY - right.magneticY,
            magneticZ - right.magneticZ,
            light - right.light,
            pressure - right.pressure,
            proximity - right.proximity
        };
    }

    inline Sensors operator / (const Sensors& right) {
        return {
            longitude / right.longitude,
            latitude / right.latitude,
            angularSpeedX / right.angularSpeedX,
            angularSpeedY / right.angularSpeedY,
            angularSpeedZ / right.angularSpeedZ,
            accelerationX / right.accelerationX,
            accelerationY / right.accelerationY,
            accelerationZ / right.accelerationZ,
            rotationX / right.rotationX,
            rotationY / right.rotationY,
            rotationZ / right.rotationZ,
            gravityX / right.gravityX,
            gravityY / right.gravityY,
            gravityZ / right.gravityZ,
            magneticX / right.magneticX,
            magneticY / right.magneticY,
            magneticZ / right.magneticZ,
            light / right.light,
            pressure / right.pressure,
            proximity / right.proximity
        };
    }

    inline float magnitude() {
        float sum = 0;

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

        return sqrt(sum);
    }

    inline static Sensors max(const Sensors& left, const Sensors& right) {
        return {
            left.longitude > right.longitude ? left.longitude : right.longitude,
            left.latitude > right.latitude  ? left.latitude : right.latitude,
            left.angularSpeedX > right.angularSpeedX ? left.angularSpeedX : right.angularSpeedX,
            left.angularSpeedY > right.angularSpeedY ? left.angularSpeedY : right.angularSpeedY,
            left.angularSpeedZ > right.angularSpeedZ ? left.angularSpeedZ : right.angularSpeedZ,
            left.accelerationX > right.accelerationX ? left.accelerationX : right.accelerationX,
            left.accelerationY > right.accelerationY ? left.accelerationY : right.accelerationY,
            left.accelerationZ > right.accelerationZ ? left.accelerationZ : right.accelerationZ,
            left.rotationX > right.rotationX ? left.rotationX : right.rotationX,
            left.rotationY > right.rotationY ? left.rotationY : right.rotationY,
            left.rotationZ > right.rotationZ ? left.longitude : right.longitude,
            left.gravityX > right.gravityX ? left.gravityX : right.gravityX,
            left.gravityY > right.gravityY ? left.gravityY : right.gravityY,
            left.gravityZ > right.gravityZ ? left.gravityZ : right.gravityZ,
            left.magneticX > right.magneticX ? left.magneticX : right.magneticX,
            left.magneticY > right.magneticY ? left.magneticY : right.magneticY,
            left.magneticZ > right.magneticZ ? left.magneticZ : right.magneticZ,
            left.light > right.light ? left.light : right.light,
            left.pressure > right.pressure ? left.pressure : right.pressure,
            left.proximity > right.proximity ? left.proximity : right.proximity
        };
    }
};

constexpr int SIZEOF_SENSORS = sizeof(Sensors);
