package com.danand.juicynoise.data

import com.google.gson.annotations.Expose

class Sensors : Cloneable {
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

    public override fun clone(): Sensors {
        val clone = Sensors()

        clone.longitude = longitude
        clone.latitude = latitude
        clone.angularSpeedX = angularSpeedX
        clone.angularSpeedY = angularSpeedY
        clone.angularSpeedZ = angularSpeedZ
        clone.accelerationX = accelerationX
        clone.accelerationY = accelerationY
        clone.accelerationZ = accelerationZ
        clone.rotationX = rotationX
        clone.rotationY = rotationY
        clone.rotationZ = rotationZ
        clone.gravityX = gravityX
        clone.gravityY = gravityY
        clone.gravityZ = gravityZ
        clone.magneticX = magneticX
        clone.magneticY = magneticY
        clone.magneticZ = magneticZ
        clone.light = light
        clone.pressure = pressure
        clone.proximity = proximity
        clone.cellSignalStrength = cellSignalStrength
        clone.wifiSignalStrength = wifiSignalStrength

        return clone
    }
}
