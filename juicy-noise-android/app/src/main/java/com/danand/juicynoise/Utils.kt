package com.danand.juicynoise

import kotlin.math.PI
import kotlin.math.abs
import kotlin.math.floor
import kotlin.math.sin
import kotlin.math.sqrt

fun magnitude(x: Float, y: Float, z: Float): Float {
    return sqrt((x * x) + (y * y) + (z * z))
}

fun lerp(from: Float, to: Float, ratio: Float): Float {
    return abs((1.0f - ratio) * from + (to * ratio))
}

fun sawtoothWave(time: Float, frequency: Int, amplitude: Float, phaseShift: Float): Float {
    val period = 1.0f / frequency
    val cycles = (time - phaseShift) / period
    val normalizedTime = cycles - floor(cycles)
    val amplitudeAtTime = amplitude * (2.0f * normalizedTime - 1.0f)

    return amplitudeAtTime
}

fun squareWave(time: Float, frequency: Int, amplitude: Float, phaseShift: Float): Float {
    val period = 1.0f / frequency
    val cycles = (time - phaseShift) / period
    val normalizedTime = cycles - floor(cycles)
    val amplitudeAtTime = if (normalizedTime < 0.5f) amplitude else -amplitude

    return amplitudeAtTime
}

fun sineWave(time: Float, frequency: Int, amplitude: Float, phaseShift: Float): Float {
    val period = 1.0f / frequency
    val cycles = (time - phaseShift) / period
    val angle = 2.0f * PI.toFloat() * cycles
    val amplitudeAtTime = amplitude * sin(angle)

    return amplitudeAtTime
}

fun exoticWave(time: Float, frequency: Int, amplitude: Float, phaseShift: Float): Float {
    val period = 1.0f / frequency
    val cycles = (time - phaseShift) / period
    val normalizedTime = cycles - floor(cycles)

    val amplitudeAtTime: Float

    if (normalizedTime <= 0.5f) {
        amplitudeAtTime = 2.0f * amplitude * normalizedTime
    } else {
        amplitudeAtTime = 2.0f * amplitude * (1.0f - normalizedTime)
    }

    return amplitudeAtTime
}

fun inverseLerp(from: Float, to: Float, value: Float): Float {
    return abs((value - from) / (to - from))
}

fun stripInt(value: Float): Float {
    val intValue = value.toInt()
    return value - intValue
}

fun sumDigits(number: Int): Int {
    var num = number
    var sum = 0

    while (num != 0) {
        val digit = num % 10
        sum += digit
        num /= 10
    }

    return sum
}

fun normalizeOnto(
    value: Float,
    fromStart: Float,
    fromEnd: Float,
    toStart: Float,
    toEnd: Float,
    ): Float {
    val normalizedValue = (value - fromStart) / (fromEnd - fromStart)
    val mappedValue = normalizedValue * (toEnd - toStart) + toStart

    return mappedValue
}
