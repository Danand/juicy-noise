package com.danand.juicynoise.synths

import com.danand.juicynoise.interfaces.Synth

import kotlin.math.floor

class SynthSawtooth : Synth {
    override fun getSample(
        time: Float,
        frequency: Int,
        amplitude: Float,
        phaseShift: Float,
    ): Float {
        val period = 1.0f / frequency
        val cycles = (time - phaseShift) / period
        val normalizedTime = cycles - floor(cycles)
        val amplitudeAtTime = amplitude * (2.0f * normalizedTime - 1.0f)

        return amplitudeAtTime
    }
}

