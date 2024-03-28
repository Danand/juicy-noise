package com.danand.juicynoise.composables

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState

@Composable
fun AlertError(
    errorState: MutableState<String?>,
) {
    if (errorState.value != null) {
        AlertDialog(
            onDismissRequest = {
                errorState.value = null
            },
            title = {
                Text("ERROR")
            },
            text = {
                Text(errorState.value.toString())
            },
            confirmButton = {
                Button(
                    onClick = {
                        errorState.value = null
                    },
                ) {
                    Text("OK")
                }
            }
        )
    }
}