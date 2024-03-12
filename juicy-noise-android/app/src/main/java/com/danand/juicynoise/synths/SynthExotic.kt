package com.danand.juicynoise.synths

import com.danand.juicynoise.interfaces.Synth

import kotlin.math.floor

class SynthExotic : Synth {
    override fun getSample(
        time: Float,
        frequency: Int,
        amplitude: Float,
        phaseShift: Float,
    ): Float {
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
}