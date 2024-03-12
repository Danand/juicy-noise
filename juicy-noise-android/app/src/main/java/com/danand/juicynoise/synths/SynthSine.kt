package com.danand.juicynoise.synths

import com.danand.juicynoise.interfaces.Synth

import kotlin.math.PI
import kotlin.math.sin

class SynthSine : Synth {
    override fun getSample(
        time: Float,
        frequency: Int,
        amplitude: Float,
        phaseShift: Float,
    ): Float {
        val period = 1.0f / frequency
        val cycles = (time - phaseShift) / period
        val angle = 2.0f * PI.toFloat() * cycles
        val amplitudeAtTime = amplitude * sin(angle)

        return amplitudeAtTime
    }
}
