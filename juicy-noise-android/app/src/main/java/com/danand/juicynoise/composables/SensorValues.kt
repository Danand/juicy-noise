@file:OptIn(
    ExperimentalMaterial3Api::class,
)

package com.danand.juicynoise.composables

import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.enumerateSensors
import com.danand.juicynoise.ui.theme.Orange

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.unit.dp

@Composable
fun SensorValues(
    sensorsState: MutableState<Sensors>,
    isShowingSensorsState: MutableState<Boolean>
) {
    Spacer(modifier = Modifier.height(24.dp))

    LabelledCheckBox(
        label = "Show sensor values",
        checked = isShowingSensorsState.value,
        onCheckedChange = {
            isShowingSensorsState.value = it
        },
    )

    if (isShowingSensorsState.value) {
        Spacer(modifier = Modifier.height(24.dp))

        enumerateSensors(sensorsState.value).forEach {
            OutlinedTextField(
                value = it.second.toString(),
                onValueChange = { },
                label = {
                    Text(
                        text = it.first,
                        fontFamily = FontFamily.Monospace,
                    )
                },
                modifier = Modifier.fillMaxWidth()
                                   .height(56.dp),
                readOnly = true,
                textStyle = TextStyle(
                    fontFamily = FontFamily.Monospace,
                ),
                colors = TextFieldDefaults.outlinedTextFieldColors(
                    disabledLabelColor = Orange,
                    focusedLabelColor = Orange,
                    errorLabelColor = Orange,
                    unfocusedLabelColor = Orange,
                )
            )

            Spacer(modifier = Modifier.height(12.dp))
        }
    }
}
