package com.danand.juicynoise.composables

import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.unit.dp

@Composable
fun ButtonDisconnect(
    isRunning: MutableState<Boolean>,
) {
    Button(
        onClick = {
            isRunning.value = false
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
        Text("Disconnect")
    }
}
