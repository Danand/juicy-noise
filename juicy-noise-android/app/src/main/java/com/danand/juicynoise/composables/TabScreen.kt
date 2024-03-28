package com.danand.juicynoise.composables

import com.danand.juicynoise.AudioOutput
import com.danand.juicynoise.data.AudioBufferSize
import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.data.SettingsState

import android.net.ConnectivityManager

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRow
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier

import com.google.android.gms.location.FusedLocationProviderClient

@Composable
fun TabScreen(
    ipState: MutableState<String>,
    portState: MutableState<UShort>,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    connectivityManager: ConnectivityManager,
    audioOutput: AudioOutput,
    isShowingSensorsState: MutableState<Boolean>,
    isShowingAudioSettings: MutableState<Boolean>,
    settingsState: SettingsState,
) {
    var tabIndex by remember { mutableStateOf(0) }

    val tabs = listOf(
        "Standalone",
        "VST",
    )

    Column(modifier = Modifier.fillMaxWidth()) {
        TabRow(selectedTabIndex = tabIndex) {
            tabs.forEachIndexed { index, title ->
                Tab(text = { Text(title) },
                    selected = tabIndex == index,
                    onClick = { tabIndex = index }
                )
            }
        }
        when (tabIndex) {
            0 -> TabStandalone(
                isRunningState,
                errorState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
                locationClient,
                audioOutput,
                isShowingSensorsState,
                isShowingAudioSettings,
                settingsState,
            )

            1 -> TabVST(
                ipState,
                portState,
                isRunningState,
                errorState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
                locationClient,
                connectivityManager,
                isShowingSensorsState,
            )
        }
    }
}
