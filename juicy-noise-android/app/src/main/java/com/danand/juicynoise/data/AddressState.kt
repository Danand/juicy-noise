package com.danand.juicynoise.data

import androidx.compose.runtime.MutableState

data class AddressState(
    val ip: MutableState<String>,
    val port: MutableState<UShort>,
)
