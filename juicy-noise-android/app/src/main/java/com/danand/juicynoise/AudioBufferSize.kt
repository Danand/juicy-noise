@file:OptIn(
    ExperimentalMaterial3Api::class,
    ExperimentalComposeUiApi::class,
)

package com.danand.juicynoise

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.ExperimentalComposeUiApi
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.focus.onFocusChanged
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

enum class AudioBufferSize(val value: Int) {
    SIZE_64(64),
    SIZE_96(96),
    SIZE_128(128),
    SIZE_256(256),
    SIZE_512(512),
    SIZE_1024(1024),
    SIZE_2048(2048),
    SIZE_4096(4096),
}

@Composable
fun AudioBufferSizeTextField(
    selectedBufferSize: AudioBufferSize,
    onItemSelected: (AudioBufferSize) -> Unit
) {
    val keyboardController = LocalSoftwareKeyboardController.current
    val expanded = remember { mutableStateOf(false) }
    val focusRequester = remember { FocusRequester() }

    Column {
        OutlinedTextField(
            value = selectedBufferSize.value.toString(),
            label = {
                Text("Audio Buffer Size")
            },
            onValueChange =  { },
            keyboardOptions = KeyboardOptions(
                keyboardType = KeyboardType.Number,
                imeAction = ImeAction.None,
            ),
            modifier = Modifier.fillMaxWidth()
                               .height(72.dp)
                               .focusRequester(focusRequester)
                               .onFocusChanged { focusState ->
                                   if (focusState.isFocused) {
                                       keyboardController?.hide()
                                       expanded.value = true
                                   } else {
                                       expanded.value = false
                                   }
                               },
        )

        DropdownMenu(
            expanded = expanded.value,
            onDismissRequest = { expanded.value = false },
        ) {
            AudioBufferSize.values().forEach { bufferSize ->
                DropdownMenuItem(
                    text = {
                        Text(
                            text = bufferSize.value.toString(),
                            style = TextStyle(
                                fontSize = 16.sp,
                                color = Color.Red,
                            ),
                            fontFamily = FontFamily.Monospace,
                        )
                    },
                    onClick = {
                        onItemSelected(bufferSize)
                        expanded.value = false
                    },
                )
            }
        }
    }
}
