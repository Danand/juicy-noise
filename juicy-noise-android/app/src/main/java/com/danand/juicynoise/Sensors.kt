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

    override fun hashCode(): Int =
        super.hashCode() xor
        longitude.hashCode() xor
        latitude.hashCode() xor
        angularSpeedX.hashCode() xor
        angularSpeedY.hashCode() xor
        angularSpeedZ.hashCode() xor
        accelerationX.hashCode() xor
        accelerationY.hashCode() xor
        accelerationZ.hashCode() xor
        rotationX.hashCode() xor
        rotationY.hashCode() xor
        rotationZ.hashCode() xor
        gravityX.hashCode() xor
        gravityY.hashCode() xor
        gravityZ.hashCode() xor
        magneticX.hashCode() xor
        magneticY.hashCode() xor
        magneticZ.hashCode() xor
        light.hashCode() xor
        pressure.hashCode() xor
        proximity.hashCode() xor
        cellSignalStrength.hashCode() xor
        wifiSignalStrength.hashCode();

    override fun equals(other: Any?): Boolean {
        return if (other is Sensors) {
            this.hashCode() == other.hashCode()
        } else {
            false
        }
    }
}
