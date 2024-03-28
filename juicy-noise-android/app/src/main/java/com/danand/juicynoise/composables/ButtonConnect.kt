package com.danand.juicynoise.composables

import com.danand.juicynoise.data.AudioBufferSize
import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.utils.runReadingLocation

import android.util.Log

import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.unit.dp

import com.google.android.gms.location.FusedLocationProviderClient
import com.google.gson.GsonBuilder

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

import java.io.DataOutputStream
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder

import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine
import kotlin.math.floor

@Composable
fun ButtonConnect(
    ip: String,
    port: UShort,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    validator: () -> Boolean
) {
    Button(
        onClick = {
            isRunningState.value = true

            runReadingLocation(
                locationClient,
                isRunningState,
                sensorsState,
            )

            startSendingSensors(
                ip,
                port,
                isRunningState,
                errorState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
            )
        },
        colors = ButtonDefaults.textButtonColors(
            containerColor = MaterialTheme.colorScheme.primaryContainer
        ),
        elevation = ButtonDefaults.buttonElevation(
            defaultElevation = 0.dp,
            pressedElevation = 16.dp,
            disabledElevation = 0.dp,
        ),
        enabled = validator()
    ) {
        if (validator()) {
            Text("Connect to $ip:$port")
        } else {
            Text("Invalid address")
        }
    }
}

fun startSendingSensors(
    ip: String,
    port: UShort,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
) {
    val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    scope.launch(Dispatchers.IO) {
        try {
            val socket = Socket(ip, port.toInt())
            val socketStream = socket.getOutputStream()
            val dataOutputStream = DataOutputStream(socketStream)

            val gson = GsonBuilder().setPrettyPrinting().create()

            while (isRunningState.value) {
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.longitude)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.latitude)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.angularSpeedX)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.angularSpeedY)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.angularSpeedZ)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.accelerationX)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.accelerationY)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.accelerationZ)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.rotationX)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.rotationY)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.rotationZ)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.gravityX)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.gravityY)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.gravityZ)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.magneticX)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.magneticY)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.magneticZ)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.light)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.pressure)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.proximity)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.cellSignalStrength)
                dataOutputStream.writeFloatWithLittleEndian(sensorsState.value.wifiSignalStrength)

                dataOutputStream.flush()

                val delayDuration = floor(audioBufferSizeState.value.value / (sampleRateState.value / 1000.0)).toLong()

                Log.d("Sensors", gson.toJson(sensorsState.value))

                delay(delayDuration)
            }

            dataOutputStream.close()
            socketStream.close()
            socket.close()
        } catch (exception: Exception) {
            errorState.value = exception.message
        }
        finally {
            isRunningState.value = false
        }
    }
}

fun DataOutputStream.writeFloatWithLittleEndian(value: Float) {
    val buffer = ByteBuffer.allocate(Float.SIZE_BYTES).order(
        ByteOrder.LITTLE_ENDIAN)

    buffer.putFloat(value)

    val bytes = buffer.array()

    this.write(bytes)
}
