package com.danand.juicynoise

import kotlinx.serialization.Serializable

@Serializable
data class Sensors(
    val longitude: Float,
    val latitude: Float,
    val accelerationX: Float,
    val accelerationY: Float,
    val accelerationZ: Float,
    val rotationX: Float,
    val rotationY: Float,
    val rotationZ: Float,
    val gravityX: Float,
    val gravityY: Float,
    val gravityZ: Float,
)
