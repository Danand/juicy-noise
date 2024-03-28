package com.danand.juicynoise.composables

import com.danand.juicynoise.AudioOutput
import com.danand.juicynoise.data.AudioBufferSize
import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.data.SettingsState

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

import com.google.android.gms.location.FusedLocationProviderClient

@Composable
fun TabStandalone(
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    audioOutput: AudioOutput,
    isShowingSensorsState: MutableState<Boolean>,
    isShowingAudioSettings: MutableState<Boolean>,
    settingsState: SettingsState,
) {
    Column(
        modifier = Modifier.padding(36.dp)
            .verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
    ) {
        BufferSettings(
            sampleRateState,
            audioBufferSizeState,
        )

        Spacer(modifier = Modifier.height(24.dp))

        if (isRunningState.value) {
            ButtonStopStandaloneAudio(
                isRunningState,
                audioOutput,
            )
        } else {
            ButtonPlayStandaloneAudio(
                isRunningState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
                locationClient,
                audioOutput,
            )
        }

        AudioSettings(
            settingsState,
            isShowingAudioSettings,
        )

        SensorValues(
            sensorsState,
            isShowingSensorsState,
        )

        AlertError(
            errorState,
        )
    }
}
