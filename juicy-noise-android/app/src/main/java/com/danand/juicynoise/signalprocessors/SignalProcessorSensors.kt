package com.danand.juicynoise.signalprocessors

import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.utils.WeightedChoice
import com.danand.juicynoise.interfaces.SignalProcessor
import com.danand.juicynoise.interfaces.Synth
import com.danand.juicynoise.utils.magnitude
import com.danand.juicynoise.utils.normalizeOnto
import com.danand.juicynoise.synths.SynthExotic
import com.danand.juicynoise.synths.SynthSawtooth
import com.danand.juicynoise.synths.SynthSine
import com.danand.juicynoise.synths.SynthSquare

import androidx.compose.runtime.MutableState

import kotlin.math.abs
import kotlin.math.max
import kotlin.random.Random

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
        ) },
        { sensorsState.value.light },
        { sensorsState.value.pressure },
        { sensorsState.value.proximity },
        { sensorsState.value.cellSignalStrength },
        { sensorsState.value.wifiSignalStrength },
    )

    private val synths: Array<Synth> = arrayOf(
        SynthSine(),
        SynthSquare(),
        SynthSawtooth(),
        SynthExotic(),
    )

    private val initRandomizedMapping = {
        Random.nextInt(0, this.sensorGetters.count())
    }

    private val mapping: Array<Int> = Array(this.synths.count()) {
        initRandomizedMapping()
    }

    private val frequencyMaxToWeights = listOf(
        700.0f to 0.6,
        2500.0f to 0.3,
        7000.0f to 0.1,
    )

    override fun process(time: Float): Float {
        val angularSpeedMagnitude = magnitude(
            this.sensorsState.value.angularSpeedX,
            this.sensorsState.value.angularSpeedY,
            this.sensorsState.value.angularSpeedZ,
        )

        if (angularSpeedMagnitude > 10) {
            randomizeMapping(this.mapping, initRandomizedMapping)
        }

        val accelerationMagnitude = magnitude(
            this.sensorsState.value.accelerationX,
            this.sensorsState.value.accelerationY,
            this.sensorsState.value.accelerationZ,
        )

        var frequencyMax = 700.0f
        val amplitudeMax = 0.9f

        var sampleValueTotal = 0.0f

        for (index in this.mapping.indices) {
            val synth = this.synths[index]
            val sensorGetter = this.sensorGetters[this.mapping[index]]

            val sensorValue = sensorGetter()

            if (accelerationMagnitude > 40) {
                frequencyMax = WeightedChoice(frequencyMaxToWeights).next()
            }

            val frequency = normalizeOnto(
                sensorValue,
                10.0f,
                1000.0f,
                40.0f,
                frequencyMax,
            ).toInt()

            val sampleValueSynth = synth.getSample(
                time,
                frequency,
                amplitudeMax,
                0.0f,
            )

            sampleValueTotal = max(sampleValueTotal, sampleValueSynth)
        }

        if ((this.sensorsState.value.light % this.sensorsState.value.magneticZ).toInt() % 3 == 0) {
            val sampleValueRandom = Random.nextFloat() * amplitudeMax
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
