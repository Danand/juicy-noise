package com.danand.juicynoise.composables

import androidx.compose.runtime.Composable
import androidx.compose.ui.text.input.KeyboardType

@Composable
fun InputFloat(
    label: String,
    value: Float,
    setter: (Float) -> Unit,
) {
    InputParse(
        label = label,
        value = value,
        setter = setter,
        parser = { it.toFloatOrNull() },
        keyboardType = KeyboardType.Number,
    )
}
