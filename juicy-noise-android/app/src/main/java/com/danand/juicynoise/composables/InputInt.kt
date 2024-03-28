package com.danand.juicynoise.composables

import androidx.compose.runtime.Composable
import androidx.compose.ui.text.input.KeyboardType

@Composable
fun InputInt(
    label: String,
    value: Int,
    setter: (Int) -> Unit,
) {
    InputParse(
        label = label,
        value = value,
        setter = setter,
        parser = { it.toIntOrNull() },
        keyboardType = KeyboardType.NumberPassword,
    )
}
