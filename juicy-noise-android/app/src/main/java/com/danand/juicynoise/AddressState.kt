package com.danand.juicynoise

import androidx.compose.runtime.MutableState

data class AddressState(
    val ip: MutableState<String>,
    val port: MutableState<UShort>,
)
