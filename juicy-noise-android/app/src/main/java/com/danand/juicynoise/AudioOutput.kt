package com.danand.juicynoise

import com.danand.juicynoise.interfaces.SignalProcessor
import com.danand.juicynoise.interfaces.Effect

import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioTrack

import kotlin.math.max

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

class AudioOutput(
    private val signalProcessors: Array<SignalProcessor>,
    private val effects: Array<Effect>,
    ) {
    private lateinit var scope: CoroutineScope

    fun play(
        samplingRate: Int,
        bufferSize: Int,
        ) {
        scope = CoroutineScope(Dispatchers.IO + SupervisorJob())

        scope.launch(Dispatchers.IO) {
            val audioTrack = AudioTrack.Builder()
                .setAudioAttributes(
                    AudioAttributes.Builder()
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .build()
                )
                .setAudioFormat(
                    AudioFormat.Builder()
                        .setEncoding(AudioFormat.ENCODING_PCM_FLOAT)
                        .setSampleRate(samplingRate)
                        .setChannelMask(AudioFormat.CHANNEL_OUT_STEREO)
                        .build()
                )
                .setTransferMode(AudioTrack.MODE_STREAM)
                .setBufferSizeInBytes(bufferSize)
                .build()

            audioTrack.play()

            val bufferStereo = FloatArray(bufferSize)

            val bufferSizeMono = bufferSize / 2

            val bufferMono = FloatArray(bufferSizeMono)

            val sampleTimeStep = 1.0f / samplingRate

            var timeElapsedSeconds = 0.0f

            while (isActive) {
                for (sampleIndex in bufferMono.indices) {
                    val sampleTime = timeElapsedSeconds + (sampleIndex * sampleTimeStep)

                    var sampleValueMax = 0.0f

                    for (signalProcessor in signalProcessors) {
                        val sampleValue = signalProcessor.process(sampleTime)
                        sampleValueMax = max(sampleValueMax, sampleValue)
                    }

                    bufferMono[sampleIndex] = sampleValueMax
                }

                // TODO: Uncomment when delay effect will be fixed.
                //for (effect in effects) {
                //    effect.process(floatArray, bufferSize)
                //}

                timeElapsedSeconds += bufferSizeMono * sampleTimeStep

                var sampleIndexStereo = 0

                for (sampleIndexMono in bufferMono.indices) {
                    bufferStereo[sampleIndexStereo] = bufferMono[sampleIndexMono]
                    bufferStereo[sampleIndexStereo + 1] = bufferMono[sampleIndexMono]

                    sampleIndexStereo += 2
                }

                audioTrack.write(
                    bufferStereo,
                    0,
                    bufferSize,
                    AudioTrack.WRITE_BLOCKING,
                )
            }

            audioTrack.stop()
        }
    }

    fun stop() {
        scope.cancel()
    }
}
