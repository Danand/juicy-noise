package com.danand.juicynoise.interfaces

interface SignalProcessor {
    fun process(time: Float): Float
}