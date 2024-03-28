package com.danand.juicynoise.composables

import com.danand.juicynoise.data.SettingsState

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun AudioSettings(
    settingsState: SettingsState,
    isShowingAudioSettings: MutableState<Boolean>,
) {
    Spacer(modifier = Modifier.height(24.dp))

    LabelledCheckBox(
        label = "Show settings",
        checked = isShowingAudioSettings.value,
        onCheckedChange = {
            isShowingAudioSettings.value = it
        },
    )

    if (isShowingAudioSettings.value) {
        InputFloat(
            "Stereo Separation",
            settingsState.stereoSeparation.value
        ) {
            settingsState.stereoSeparation.value = it
        }

        InputFloat(
            "Frequency Min",
            settingsState.frequencyMin.value
        ) {
            settingsState.frequencyMin.value = it
        }

        InputFloat(
            "Sensitivity A",
            settingsState.sensitivityA.value
        ) {
            settingsState.sensitivityA.value = it
        }

        InputFloat(
            "Sensitivity B",
            settingsState.sensitivityB.value,
        ) {
            settingsState.sensitivityB.value = it
        }

        InputFloat(
            "Sensitivity C",
            settingsState.sensitivityC.value,
        ) {
            settingsState.sensitivityC.value = it
        }

        InputFloat(
            "Sensitivity D",
            settingsState.sensitivityD.value,
        ) {
            settingsState.sensitivityD.value = it
        }

        InputInt(
            "Rhythm Seed A",
            settingsState.rhythmSeedA.value,
        ) {
            settingsState.rhythmSeedA.value = it
        }

        InputInt(
            "Rhythm Seed B",
            settingsState.rhythmSeedB.value,
        ) {
            settingsState.rhythmSeedB.value = it
        }
    }
}
