package com.danand.juicynoise.synths

import com.danand.juicynoise.interfaces.Synth

import kotlin.math.floor

class SynthSquare : Synth {
    override fun getSample(
        time: Float,
        frequency: Int,
        amplitude: Float,
        phaseShift: Float,
    ): Float {
        val period = 1.0f / frequency
        val cycles = (time - phaseShift) / period
        val normalizedTime = cycles - floor(cycles)
        val amplitudeAtTime = if (normalizedTime < 0.5f) amplitude else -amplitude

        return amplitudeAtTime
    }
}