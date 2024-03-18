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

            val bufferMonoLeft = FloatArray(bufferSizeMono)
            val bufferMonoRight = FloatArray(bufferSizeMono)

            var timeElapsedSeconds = 0.0f

            while (isActive) {
                processChannel(
                    0,
                    samplingRate,
                    timeElapsedSeconds,
                    bufferMonoLeft,
                )

                processChannel(
                    1,
                    samplingRate,
                    timeElapsedSeconds,
                    bufferMonoRight,
                )

                timeElapsedSeconds += bufferSizeMono * (1.0f / samplingRate)

                processStereo(
                    bufferMonoLeft,
                    bufferMonoRight,
                    bufferStereo,
                )

                for (effect in effects) {
                    effect.process(bufferStereo)
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

    private fun processChannel(
        channelIndex: Int,
        samplingRate: Int,
        time: Float,
        buffer: FloatArray,
        ) {
        val sampleTimeStep = 1.0f / samplingRate

        for (sampleIndex in buffer.indices) {
            val sampleTime = time + (sampleIndex * sampleTimeStep)

            var sampleValueMax = 0.0f

            for (signalProcessor in this.signalProcessors) {
                if (signalProcessor.getChannel() == channelIndex) {
                    val sampleValue = signalProcessor.process(sampleTime)
                    sampleValueMax = max(sampleValueMax, sampleValue)
                }
            }

            buffer[sampleIndex] = sampleValueMax
        }
    }

    private fun processStereo(
        bufferMonoLeft: FloatArray,
        bufferMonoRight: FloatArray,
        bufferStereo: FloatArray,
        ) {
        var sampleIndexStereo = 0

        val channelMergeFactor = 0.125f

        for (sampleIndexMono in bufferMonoLeft.indices) {
            val sampleLeft = bufferMonoLeft[sampleIndexMono]
            val sampleRight = bufferMonoRight[sampleIndexMono]

            bufferStereo[sampleIndexStereo] = (sampleLeft * (1 - channelMergeFactor)) + (sampleRight * channelMergeFactor)
            bufferStereo[sampleIndexStereo + 1] = (sampleRight * (1 - channelMergeFactor)) + (sampleLeft * channelMergeFactor)

            sampleIndexStereo += 2
        }
    }
}
