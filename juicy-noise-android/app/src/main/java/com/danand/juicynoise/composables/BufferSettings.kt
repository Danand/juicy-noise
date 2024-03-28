package com.danand.juicynoise.composables

import com.danand.juicynoise.data.AudioBufferSize

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun BufferSettings(
    sampleRateState: MutableState<Int>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
) {
    InputInt(
        "Sample Rate",
        sampleRateState.value
    ) {
        sampleRateState.value = it
    }

    Spacer(modifier = Modifier.height(16.dp))

    AudioBufferSizeTextField(
        selectedBufferSize = audioBufferSizeState.value,
        onItemSelected = { audioBufferSizeState.value = it }
    )
}
