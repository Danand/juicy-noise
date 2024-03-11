package com.danand.juicynoise.signalprocessors

import androidx.compose.runtime.MutableState

import com.danand.juicynoise.Sensors
import com.danand.juicynoise.exoticWave
import com.danand.juicynoise.interfaces.SignalProcessor
import com.danand.juicynoise.magnitude
import com.danand.juicynoise.normalizeOnto
import com.danand.juicynoise.sineWave
import com.danand.juicynoise.squareWave
import com.danand.juicynoise.sawtoothWave
import kotlin.math.abs

import kotlin.math.max
import kotlin.random.Random

typealias Synth = (time: Float, frequency: Int, amplitude: Float, phaseShift: Float) -> Float

class SignalProcessorSensors(private val sensorsState: MutableState<Sensors>) : SignalProcessor {
    private val sensorGetters: Array<() -> Float> = arrayOf(
        { sensorsState.value.longitude },
        { sensorsState.value.latitude },
        { magnitude(
            this.sensorsState.value.angularSpeedX,
            this.sensorsState.value.angularSpeedY,
            this.sensorsState.value.angularSpeedZ,
        ) },
        { magnitude(
            this.sensorsState.value.accelerationX,
            this.sensorsState.value.accelerationY,
            this.sensorsState.value.accelerationZ,
        ) },
        { abs(sensorsState.value.rotationX) },
        { abs(sensorsState.value.rotationY) },
        { abs(sensorsState.value.rotationZ) },
        { magnitude(
            this.sensorsState.value.gravityX,
            this.sensorsState.value.gravityY,
            this.sensorsState.value.gravityZ,
        ) },
        { magnitude(
            this.sensorsState.value.magneticX,
            this.sensorsState.value.magneticY,
            this.sensorsState.value.magneticZ,
        )},
        { sensorsState.value.light },
        { sensorsState.value.pressure },
        { sensorsState.value.proximity },
        { sensorsState.value.cellSignalStrength },
        { sensorsState.value.wifiSignalStrength },
    )

    private val synths: Array<Synth> = arrayOf(
        ::sineWave,
        ::squareWave,
        ::sawtoothWave,
        ::exoticWave,
    )

    private var mapping: Array<Int> = Array(this.synths.count()) {
        Random.nextInt(0, this.sensorGetters.count())
    }

    override fun process(time: Float): Float {
        val angularSpeedXMagnitude = magnitude(
            this.sensorsState.value.angularSpeedX,
            this.sensorsState.value.angularSpeedY,
            this.sensorsState.value.angularSpeedZ,
        )

        if (angularSpeedXMagnitude > 10) {
            randomizeMapping(this.mapping) {
                Random.nextInt(0, this.sensorGetters.count())
            }
        }

        var sampleValueTotal = 0.0f

        for (index in this.mapping.indices) {
            val synth = this.synths[index]
            val sensorGetter = this.sensorGetters[this.mapping[index]]

            val sensorValue = sensorGetter()

            val frequency = normalizeOnto(
                sensorValue,
                10.0f,
                1000.0f,
                40.0f,
                700.0f,
            ).toInt()

            val sampleValueSynth = synth(
                time,
                frequency,
                0.9f,
                0.0f,
            )

            sampleValueTotal = max(sampleValueTotal, sampleValueSynth)
        }

        if ((this.sensorsState.value.light % this.sensorsState.value.magneticZ).toInt() % 3 == 0) {
            val sampleValueRandom = Random.nextFloat()
            sampleValueTotal = max(sampleValueTotal, sampleValueRandom)
        }

        return sampleValueTotal
    }

    private fun randomizeMapping(mapping: Array<Int>, init: () -> Int) {
        for (index in mapping.indices) {
            mapping[index] = init()
        }
    }
}