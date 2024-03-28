package com.danand.juicynoise.composables

import com.danand.juicynoise.AudioOutput

import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.unit.dp

@Composable
fun ButtonStopStandaloneAudio(
    isRunning: MutableState<Boolean>,
    audioOutput: AudioOutput,
) {
    Button(
        onClick = {
            isRunning.value = false
            audioOutput.stop()
        },
        colors = ButtonDefaults.textButtonColors(
            containerColor = MaterialTheme.colorScheme.primaryContainer
        ),
        elevation = ButtonDefaults.buttonElevation(
            defaultElevation = 0.dp,
            pressedElevation = 16.dp,
            disabledElevation = 0.dp,
        ),
    ) {
        Text("Stop")
    }
}