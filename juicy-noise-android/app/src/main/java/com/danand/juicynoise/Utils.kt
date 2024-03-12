package com.danand.juicynoise

import kotlin.math.abs
import kotlin.math.sqrt

fun magnitude(x: Float, y: Float, z: Float): Float {
    return sqrt((x * x) + (y * y) + (z * z))
}

fun lerp(from: Float, to: Float, ratio: Float): Float {
    return abs((1.0f - ratio) * from + (to * ratio))
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
