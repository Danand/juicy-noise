@file:OptIn(
    ExperimentalMaterial3Api::class,
)

package com.danand.juicynoise.composables

import com.danand.juicynoise.data.AudioBufferSize
import com.danand.juicynoise.data.Sensors

import android.net.ConnectivityManager
import android.net.LinkProperties

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.core.text.isDigitsOnly

import com.google.android.gms.location.FusedLocationProviderClient

import java.net.InetSocketAddress
import java.net.Socket

@Composable
fun TabVST(
    ipState: MutableState<String>,
    portState: MutableState<UShort>,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    connectivityManager: ConnectivityManager,
    isShowingSensorsState: MutableState<Boolean>,
) {
    LaunchedEffect(portState) {
        val subnet = findSubnet(connectivityManager)
        val suggestedIp = findFirstAddressWithOpenedPort(subnet, portState.value)

        if (suggestedIp != null) {
            ipState.value = suggestedIp
        }
    }

    Column(
        modifier = Modifier.padding(36.dp)
                           .verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
    ) {
        OutlinedTextField(
            value = ipState.value,
            onValueChange = {
                if (checkIsValidIp(it)) {
                    ipState.value = it
                }
            },
            label = {
                Text("Local IP")
            },
            keyboardOptions = KeyboardOptions(
                keyboardType = KeyboardType.NumberPassword
            ),
            isError = checkIsValidIp(ipState.value) == false,
            modifier = Modifier.fillMaxWidth()
                               .height(72.dp),
        )

        Spacer(modifier = Modifier.height(16.dp))

        OutlinedTextField(
            value = portState.value.toString(),
            onValueChange = {
                if (checkIsValidPort(it)) {
                    portState.value = it.toUShort()
                }
            },
            label = {
                Text("Port")
            },
            keyboardOptions = KeyboardOptions(
                keyboardType = KeyboardType.Number,
            ),
            modifier = Modifier.fillMaxWidth()
                               .height(72.dp),
        )

        Spacer(modifier = Modifier.height(16.dp))

        BufferSettings(
            sampleRateState,
            audioBufferSizeState,
        )

        Spacer(modifier = Modifier.height(24.dp))

        if (isRunningState.value) {
            ButtonDisconnect(
                isRunningState,
            )
        } else {
            ButtonConnect(
                ipState.value,
                portState.value,
                isRunningState,
                errorState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
                locationClient,
            ) {
                checkIsValidIp(ipState.value)
            }
        }

        SensorValues(
            sensorsState,
            isShowingSensorsState,
        )

        AlertError(
            errorState,
        )
    }
}

fun checkIsValidPort(value: String): Boolean  =
    value.isDigitsOnly() &&
            (value.toUShortOrNull() ?: 0.toUShort()) in UShort.MIN_VALUE..UShort.MAX_VALUE

fun checkIsValidIp(value: String): Boolean {
    val pattern: Regex = """^([01]?\d{1,2}|2[0-4]\d|25[0-5])\.([01]?\d{1,2}|2[0-4]\d|25[0-5])\.([01]?\d{1,2}|2[0-4]\d|25[0-5])\.([01]?\d{1,2}|2[0-4]\d|25[0-5])${'$'}""".toRegex()
    return pattern.matches(value)
}

fun findFirstAddressWithOpenedPort(subnet: String?, port: UShort): String? {
    if (subnet == null) {
        return null
    }

    for (subnetPostfix in 1..255) {
        val address = "$subnet.$subnetPostfix"

        val socket = Socket()

        try {
            socket.connect(
                InetSocketAddress(
                    address,
                    port.toInt(),
                ),
                200
            )

            return address
        } catch (_ : Exception) {
        } finally {
            socket.close()
        }
    }

    return null
}

fun findSubnet(connectivityManager: ConnectivityManager): String? {
    val linkProperties: LinkProperties =  connectivityManager.getLinkProperties(connectivityManager.activeNetwork) as LinkProperties
    val linkAddress = linkProperties.linkAddresses.firstOrNull { it?.address?.hostAddress != null } ?: return null

    val addressParts = linkAddress.address.hostAddress!!.split(".")

    if (addressParts.count() > 2) {
        return addressParts.subList(0, 2).joinToString(".")
    }

    return null
}
