package com.danand.juicynoise.composables

import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.requiredHeight
import androidx.compose.foundation.layout.size
import androidx.compose.material.ripple.rememberRipple
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Checkbox
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.unit.dp

@Composable
fun LabelledCheckBox(
    checked: Boolean,
    onCheckedChange: ((Boolean) -> Unit),
    label: String,
    modifier: Modifier = Modifier
) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.Start,
        modifier = modifier.clip(MaterialTheme.shapes.small)
                           .clickable(
                               indication = rememberRipple(color = MaterialTheme.colorScheme.primary),
                               interactionSource = remember { MutableInteractionSource() },
                               onClick = { onCheckedChange(!checked) },
                           )
                           .requiredHeight(ButtonDefaults.MinHeight)
                           .padding(4.dp),
    ) {
        Checkbox(
            checked = checked,
            onCheckedChange = null,
        )

        Spacer(Modifier.size(6.dp))

        Text(
            text = label,
        )
    }
}
