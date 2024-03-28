package com.danand.juicynoise.composables

import com.danand.juicynoise.AudioOutput
import com.danand.juicynoise.data.AudioBufferSize
import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.utils.runReadingLocation

import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.unit.dp

import com.google.android.gms.location.FusedLocationProviderClient

@Composable
fun ButtonPlayStandaloneAudio(
    isRunningState: MutableState<Boolean>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    audioOutput: AudioOutput,
) {
    Button(
        onClick = {
            isRunningState.value = true

            runReadingLocation(
                locationClient,
                isRunningState,
                sensorsState,
            )

            audioOutput.play(
                sampleRateState.value,
                audioBufferSizeState.value.value,
            )
        },
        colors = ButtonDefaults.textButtonColors(
            containerColor = MaterialTheme.colorScheme.primaryContainer
        ),
        elevation = ButtonDefaults.buttonElevation(
            defaultElevation = 0.dp,
            pressedElevation = 16.dp,
            disabledElevation = 0.dp,
        ),
        enabled = true
    ) {
        Text("Play")
    }
}
