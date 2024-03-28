@file:OptIn(
    ExperimentalMaterial3Api::class,
    ExperimentalComposeUiApi::class,
)

package com.danand.juicynoise.composables

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.ExperimentalComposeUiApi
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp

@Composable
fun <T>InputParse(
    label: String,
    value: T,
    setter: (T) -> Unit,
    parser: (String) -> T?,
    keyboardType: KeyboardType,
) {
    Spacer(modifier = Modifier.height(16.dp))

    var input by remember { mutableStateOf(value.toString()) }
    var isValid by remember { mutableStateOf(true) }

    val keyboardController = LocalSoftwareKeyboardController.current

    OutlinedTextField(
        value = input,
        onValueChange = {
            input = it
            isValid = parser(it) != null
        },
        keyboardActions = KeyboardActions(
            onDone = {
                val valueParsed = parser(input)

                if (valueParsed == null) {
                    input = value.toString()
                } else {
                    setter(valueParsed)
                    input = valueParsed.toString()
                }

                keyboardController?.hide()
            }
        ),
        label = {
            Text(label)
        },
        singleLine = true,
        keyboardOptions = KeyboardOptions.Default.copy(
            keyboardType = keyboardType,
        ),
        modifier = Modifier.fillMaxWidth()
                           .height(72.dp),
        colors = TextFieldDefaults.outlinedTextFieldColors(
            focusedBorderColor = if (isValid) {
                MaterialTheme.colorScheme.primary
            } else {
                MaterialTheme.colorScheme.error
            }
        )
    )
}
