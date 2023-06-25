package com.danand.juicynoise

import androidx.compose.runtime.MutableState
import kotlinx.serialization.Serializable

data class AddressState(
    val ip: MutableState<String>,
    val port: MutableState<String>,
)
