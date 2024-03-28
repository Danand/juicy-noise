package com.danand.juicynoise.utils

import com.danand.juicynoise.configs.READING_LOCATION_INTERVAL
import com.danand.juicynoise.data.Sensors

import android.annotation.SuppressLint
import android.location.Location

import androidx.compose.runtime.MutableState

import com.google.android.gms.location.FusedLocationProviderClient

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine

fun runReadingLocation(
    locationClient: FusedLocationProviderClient,
    isRunningState: MutableState<Boolean>,
    sensorsState: MutableState<Sensors>,
) {
    val scope = CoroutineScope(
        Dispatchers.Main + SupervisorJob()
    )

    scope.launch(
        Dispatchers.Main) {
        while (isRunningState.value) {
            val location = readLocation(locationClient)

            if (location != null) {
                sensorsState.value.longitude = location.longitude.toFloat()
                sensorsState.value.latitude = location.latitude.toFloat()
            }

            delay(READING_LOCATION_INTERVAL)
        }
    }
}

@SuppressLint("MissingPermission")
suspend fun readLocation(locationClient: FusedLocationProviderClient): Location? = suspendCoroutine { continuation ->
    locationClient.lastLocation.addOnCompleteListener { task ->
        if (task.exception == null) {
            continuation.resume(task.result)
        } else {
            throw task.exception!!
        }
    }
}
