package com.danand.juicynoise.data

import androidx.compose.runtime.MutableState

data class SettingsState(
    val stereoSeparation: MutableState<Float>,
    val frequencyMin: MutableState<Float>,
    val sensitivityA: MutableState<Float>,
    val sensitivityB: MutableState<Float>,
    val sensitivityC: MutableState<Float>,
    val sensitivityD: MutableState<Float>,
    val rhythmSeedA: MutableState<Int>,
    val rhythmSeedB: MutableState<Int>,
)
