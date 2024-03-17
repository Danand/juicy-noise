package com.danand.juicynoise.signalprocessors

import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.interfaces.SignalProcessor
import com.danand.juicynoise.interfaces.Synth
import com.danand.juicynoise.utils.magnitude
import com.danand.juicynoise.utils.normalizeOnto
import com.danand.juicynoise.utils.weightedChoice
import com.danand.juicynoise.synths.SynthSawtooth
import com.danand.juicynoise.synths.SynthSine
import com.danand.juicynoise.synths.SynthSquare

import androidx.compose.runtime.MutableState

import kotlin.math.abs
import kotlin.math.max
import kotlin.math.min
import kotlin.math.tanh
import kotlin.random.Random

class SignalProcessorSensors(
    private val sensorsState: MutableState<Sensors>,
    private val channel: Int,
    ) : SignalProcessor {
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
    )

    private val initRandomizedPhases = {
        Random.nextFloat()
    }

    private val phases: Array<Float> = Array(this.synths.count()) {
        initRandomizedPhases()
    }

    private val mappingIndices: Array<Int> = Array(this.sensorGetters.count()) {
        it
    }

    private val mapping: IntArray = initializeMapping(this.synths.count(), mappingIndices)

    private val sensorGetterMinValues = FloatArray(this.sensorGetters.count())

    private val sensorGetterMaxValues = FloatArray(this.sensorGetters.count())

    private val frequencyMaxToWeights = mapOf(
        30.0f to 0.2,
        50.0f to 0.3,
        75.0f to 0.5,
        110.0f to 0.3,
        250.0f to 0.5,
        650.0f to 0.3,
        1000.0f to 0.1,
        9000.0f to 0.05,
    )

    private var frequenciesMax: Array<Float> = Array(this.synths.count()) {
        Random.nextInt(35, 70).toFloat()
    }

    private var weirdEffectDivider = 9

    override fun getChannel(): Int {
        return this.channel
    }

    override fun process(time: Float): Float {
        val angularSpeedMagnitude = magnitude(
            this.sensorsState.value.angularSpeedX,
            this.sensorsState.value.angularSpeedY,
            this.sensorsState.value.angularSpeedZ,
        )

        if (angularSpeedMagnitude > 10) {
            randomizeMapping(this.mapping, this.mappingIndices)
        }

        val accelerationMagnitude = magnitude(
            this.sensorsState.value.accelerationX,
            this.sensorsState.value.accelerationY,
            this.sensorsState.value.accelerationZ,
        )

        val amplitudeMax = 0.9f

        var sampleValueTotal = 0.0f

        for (index in this.mapping.indices) {
            val synth = this.synths[index]
            val sensorGetter = this.sensorGetters[this.mapping[index]]

            val sensorValue = sensorGetter()

            if (accelerationMagnitude > 40) {
                this.frequenciesMax[index] = weightedChoice(frequencyMaxToWeights)
            }

            if (angularSpeedMagnitude > 15) {
                weirdEffectDivider = Random.nextInt(1, 31)
            }

            val sensorGetterMinValue = this.sensorGetterMinValues[this.mapping[index]]
            this.sensorGetterMinValues[this.mapping[index]] = min(sensorGetterMinValue, sensorValue)

            val sensorGetterMaxValue = this.sensorGetterMaxValues[this.mapping[index]]
            this.sensorGetterMaxValues[this.mapping[index]] = max(sensorGetterMaxValue, sensorValue)

            var frequencyMax = this.frequenciesMax[index]

            if ((sensorValue * 10000.0f).toInt() % weirdEffectDivider == 0) {
                frequencyMax += (this.frequenciesMax.random() * tanh(time))
            }

            val frequency = normalizeOnto(
                sensorValue,
                sensorGetterMinValue,
                sensorGetterMaxValue,
                35.0f,
                frequencyMax,
            ).toInt()

            if (accelerationMagnitude > 30) {
                randomizePhases(this.phases) {
                    initRandomizedPhases()
                }
            }

            val phase = this.phases[index]

            val sampleValueSynth = synth.getSample(
                time,
                frequency,
                amplitudeMax,
                phase,
            )

            sampleValueTotal = max(sampleValueTotal, sampleValueSynth)
        }

        if ((this.sensorsState.value.light % (this.sensorsState.value.magneticZ * 10.0f)).toInt() % 3 == 0) {
            val sampleValueRandom = Random.nextFloat() * amplitudeMax
            sampleValueTotal = max(sampleValueTotal, sampleValueRandom)
        }

        return sampleValueTotal
    }

    private fun initializeMapping(amount: Int, mappingIndices: Array<Int>): IntArray {
        val mapping = IntArray(amount)
        randomizeMapping(mapping, mappingIndices)
        return mapping
    }

    private fun randomizeMapping(mapping: IntArray, mappingIndices: Array<Int>) {
        mappingIndices.shuffle()

        for (index in mapping.indices) {
            mapping[index] = mappingIndices[index]
        }
    }

    private fun randomizePhases(phases: Array<Float>, init: () -> Float) {
        for (index in phases.indices) {
            phases[index] = init()
        }
    }
}
