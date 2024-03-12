package com.danand.juicynoise.interfaces

interface Synth {
    fun getSample(
        time: Float,
        frequency: Int,
        amplitude: Float,
        phaseShift: Float,
    ): Float
}
