package com.danand.juicynoise.effects

import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.interfaces.Effect
import com.danand.juicynoise.utils.magnitude

import androidx.compose.runtime.MutableState

import kotlin.math.max
import kotlin.math.ceil
import kotlin.math.abs

class EffectDelay(
    private val sensorsState: MutableState<Sensors>,
    ) : Effect {
    private val buffers: ArrayList<FloatArray> = ArrayList()

    private var magneticMagnitudeMax: Float = 0.0f

    override fun process(buffer: FloatArray, bufferSize: Int) {
        val delayMilliseconds = max(0.0f, 5000.0f - this.sensorsState.value.light)

        val magneticMagnitude = magnitude(
            this.sensorsState.value.magneticX,
            this.sensorsState.value.magneticY,
            this.sensorsState.value.magneticZ,
        )

        magneticMagnitudeMax = max(magneticMagnitudeMax, magneticMagnitude)

        val dry = magneticMagnitude / magneticMagnitudeMax
        val wet = 1 - dry

        val bufferAmount = ceil(delayMilliseconds / bufferSize).toInt()

        val bufferCount = this.buffers.count()

        val skipBuffersCount = bufferCount - bufferAmount

        if (skipBuffersCount < 0) {
            for (i in 0..abs(skipBuffersCount)) {
                this.buffers.add(FloatArray(bufferSize))
            }
        }

        val lastBuffer = this.buffers[0]

        for (sampleIndex in buffer.indices) {
            lastBuffer[sampleIndex] = buffer[sampleIndex]
        }

        for (bufferIndex in this.buffers.indices) {
            if (bufferIndex < skipBuffersCount) {
                continue
            }

            if (bufferIndex < 1) {
                continue
            }

            val bufferSaved = this.buffers[bufferIndex]

            val bufferMix = bufferIndex / bufferAmount.toFloat()

            for (sampleIndex in buffer.indices) {
                val delayedSample = bufferSaved[sampleIndex] * bufferMix
                buffer[sampleIndex] = (buffer[sampleIndex] * dry) + (delayedSample * wet)
            }
        }

        for (bufferIndex in 0 until this.buffers.lastIndex) {
            this.buffers[bufferIndex] = this.buffers[bufferIndex + 1]
        }

        this.buffers[this.buffers.lastIndex] = lastBuffer
    }
}
