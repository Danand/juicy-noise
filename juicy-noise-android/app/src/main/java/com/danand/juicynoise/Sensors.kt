package com.danand.juicynoise

import kotlinx.serialization.Serializable

@Serializable
class Sensors {
    var longitude: Float = 0f
    var latitude: Float = 0f
    var angularSpeedX: Float = 0f
    var angularSpeedY: Float = 0f
    var angularSpeedZ: Float = 0f
    var accelerationX: Float = 0f
    var accelerationY: Float = 0f
    var accelerationZ: Float = 0f
    var rotationX: Float = 0f
    var rotationY: Float = 0f
    var rotationZ: Float = 0f
    var gravityX: Float = 0f
    var gravityY: Float = 0f
    var gravityZ: Float = 0f
    var magneticX: Float = 0f
    var magneticY: Float = 0f
    var magneticZ: Float = 0f
    var light: Float = 0f
    var pressure: Float = 0f
    var proximity: Float = 0f
}
