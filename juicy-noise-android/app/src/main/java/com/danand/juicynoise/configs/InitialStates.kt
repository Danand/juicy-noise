package com.danand.juicynoise.configs

import androidx.compose.runtime.mutableStateOf
import com.danand.juicynoise.data.AddressState
import com.danand.juicynoise.data.SettingsState

fun createAddressState(): AddressState = AddressState(
    ip = mutableStateOf("192.168.0.128"),
    port = mutableStateOf(6660u),
)

fun createSettingsState(): SettingsState = SettingsState(
    stereoSeparation = mutableStateOf(0.875f),
    frequencyMin = mutableStateOf(35.0f),
    sensitivityA = mutableStateOf(6.0f),
    sensitivityB = mutableStateOf(40.0f),
    sensitivityC = mutableStateOf(15.0f),
    sensitivityD = mutableStateOf(30.0f),
    rhythmSeedA = mutableStateOf(31),
    rhythmSeedB = mutableStateOf(3),
)

val frequencyMaxToWeights = mapOf(
    30.0f to 0.2,
    50.0f to 0.3,
    75.0f to 0.5,
    110.0f to 0.3,
    250.0f to 0.5,
    650.0f to 0.3,
    1000.0f to 0.1,
    9000.0f to 0.05,
)
