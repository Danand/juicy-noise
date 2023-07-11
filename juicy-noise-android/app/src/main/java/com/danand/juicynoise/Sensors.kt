package com.danand.juicynoise

import com.google.gson.annotations.Expose

class Sensors {
    @Expose var longitude: Float = 0f
    @Expose var latitude: Float = 0f
    @Expose var angularSpeedX: Float = 0f
    @Expose var angularSpeedY: Float = 0f
    @Expose var angularSpeedZ: Float = 0f
    @Expose var accelerationX: Float = 0f
    @Expose var accelerationY: Float = 0f
    @Expose var accelerationZ: Float = 0f
    @Expose var rotationX: Float = 0f
    @Expose var rotationY: Float = 0f
    @Expose var rotationZ: Float = 0f
    @Expose var gravityX: Float = 0f
    @Expose var gravityY: Float = 0f
    @Expose var gravityZ: Float = 0f
    @Expose var magneticX: Float = 0f
    @Expose var magneticY: Float = 0f
    @Expose var magneticZ: Float = 0f
    @Expose var light: Float = 0f
    @Expose var pressure: Float = 0f
    @Expose var proximity: Float = 0f
    @Expose var cellSignalStrength: Float = 0f
    @Expose var wifiSignalStrength: Float = 0f
}
