#ifndef SENSORS_H
#define SENSORS_H

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

#endif
