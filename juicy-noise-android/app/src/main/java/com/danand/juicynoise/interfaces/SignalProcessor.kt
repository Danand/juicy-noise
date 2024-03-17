package com.danand.juicynoise.interfaces

interface SignalProcessor {
    fun getChannel(): Int

    fun process(time: Float): Float
}
