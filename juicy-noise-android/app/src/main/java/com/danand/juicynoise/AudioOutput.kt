package com.danand.juicynoise

import com.danand.juicynoise.interfaces.SignalProcessor

import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioTrack
import com.danand.juicynoise.interfaces.Effect

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.cancel
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

class AudioOutput(
    private val signalProcessor: SignalProcessor,
    private val effect: Effect,
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

            val floatArray = FloatArray(bufferSize)

            val sampleTimeStep = 1.0f / samplingRate

            val timeStart = System.currentTimeMillis() * 1000.0f

            var timeCurrent = timeStart

            while (isActive) {
                val timeElapsedSeconds = timeCurrent - timeStart

                for (i in floatArray.indices) {
                    val sampleTime = timeElapsedSeconds + (i * sampleTimeStep)
                    val value = signalProcessor.process(sampleTime)

                    floatArray[i] = value
                }

                // TODO: Uncomment when delay effect will be fixed.
                //effect.process(floatArray, bufferSize)

                audioTrack.write(floatArray, 0, bufferSize, AudioTrack.WRITE_NON_BLOCKING)

                timeCurrent += bufferSize * sampleTimeStep
            }

            audioTrack.stop()
        }
    }

    fun stop() {
        scope.cancel()
    }
}
