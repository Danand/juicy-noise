@file:OptIn(
    ExperimentalMaterial3Api::class,
)

package com.danand.juicynoise

import android.annotation.SuppressLint
import android.app.Activity
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.location.Location
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.ButtonDefaults.textButtonColors
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.core.text.isDigitsOnly
import com.danand.juicynoise.ui.theme.JuicyNoiseTheme
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import java.io.DataOutputStream
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder
import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine
import kotlin.random.Random

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        ensureAllPermissions(this)

        val locationClient: FusedLocationProviderClient = LocationServices.getFusedLocationProviderClient(this)

        setContent {
            JuicyNoiseTheme() {
                val addressState = remember { createDefaultState() }
                val isRunningState = remember { mutableStateOf(false) }
                val errorState = remember { mutableStateOf<String?>(null) }

                ColumnMain(
                    addressState.ip,
                    addressState.port,
                    isRunningState,
                    errorState,
                    locationClient,
                )
            }
        }
    }
}

@Composable
fun ColumnMain(
    ipState: MutableState<String>,
    portState: MutableState<String>,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    locationClient: FusedLocationProviderClient? = null,
) {
    Column(
        modifier = Modifier.padding(36.dp),
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
            modifier = Modifier
                .fillMaxWidth()
                .height(72.dp),
        )

        Spacer(modifier = Modifier.height(16.dp))

        OutlinedTextField(
            value = portState.value,
            onValueChange = {
                if (checkIsValidPort(it)) {
                    portState.value = it
                }
            },
            label = {
                Text("Port")
            },
            keyboardOptions = KeyboardOptions(
                keyboardType = KeyboardType.Number,
            ),
            isError = checkIsValidPort(portState.value) == false,
            modifier = Modifier
                .fillMaxWidth()
                .height(72.dp),
        )

        Spacer(modifier = Modifier.height(24.dp))

        if (isRunningState.value) {
            ButtonDisconnect(
                isRunningState,
            )
        } else {
            ButtonConnect(
                ipState.value,
                portState.value.toUShort(),
                isRunningState,
                errorState,
                locationClient,
            ) {
                checkIsValidIp(ipState.value) &&
                checkIsValidPort(portState.value)
            }
        }

        if (errorState.value != null) {
            AlertDialog(
                onDismissRequest = {
                    errorState.value = null
                },
                title = {
                    Text("ERROR")
                },
                text = {
                    Text(errorState.value.toString())
                },
                confirmButton = {
                    Button(
                        onClick = {
                            errorState.value = null
                        },
                    ) {
                        Text("OK")
                    }
                }
            )
        }
    }
}

fun checkIsValidPort(value: String): Boolean  =
    value.isDigitsOnly() &&
    (value.toUShortOrNull() ?: 0.toUShort()) in UShort.MIN_VALUE..UShort.MAX_VALUE

fun checkIsValidIp(value: String): Boolean {
    val pattern: Regex = """^([01]?\d{1,2}|2[0-4]\d|25[0-5])\.([01]?\d{1,2}|2[0-4]\d|25[0-5])\.([01]?\d{1,2}|2[0-4]\d|25[0-5])\.([01]?\d{1,2}|2[0-4]\d|25[0-5])${'$'}""".toRegex()
    return pattern.matches(value)
}

@Composable
fun ButtonConnect(
    ip: String,
    port: UShort,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    locationClient: FusedLocationProviderClient?,
    validator: () -> Boolean
) {
    Button(
        onClick = {
            startSendingSensors(
                ip,
                port,
                isRunningState,
                errorState,
                locationClient,
            )
        },
        colors = textButtonColors(
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

@Composable
fun ButtonDisconnect(
    isRunning: MutableState<Boolean>,
) {
    Button(
        onClick = {
            isRunning.value = false
        },
        colors = textButtonColors(
            containerColor = MaterialTheme.colorScheme.primaryContainer
        ),
        elevation = ButtonDefaults.buttonElevation(
            defaultElevation = 0.dp,
            pressedElevation = 16.dp,
            disabledElevation = 0.dp,
        ),
    ) {
        Text("Disconnect")
    }
}

@Preview(showBackground = true)
@Composable
fun Preview() {
    JuicyNoiseTheme {
        val addressState = remember { createDefaultState() }
        val isRunningState = remember { mutableStateOf(false) }
        val errorState = remember { mutableStateOf<String?>(null) }

        ColumnMain(
            addressState.ip,
            addressState.port,
            isRunningState,
            errorState,
        )
    }
}

fun createDefaultState(): AddressState = AddressState(
    ip = mutableStateOf("192.168.0.1"),
    port = mutableStateOf("6660"),
)

fun startSendingSensors(
    ip: String,
    port: UShort,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    locationClient: FusedLocationProviderClient?,
) {
    val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    scope.launch(Dispatchers.IO) {
        isRunningState.value = true

        try {
            val socket = Socket(ip, port.toInt())
            val socketStream = socket.getOutputStream()
            val dataOutputStream = DataOutputStream(socketStream)

            while (isRunningState.value) {
                val sensors: Sensors = readSensors(locationClient)

                dataOutputStream.writeFloatWithLittleEndian(sensors.longitude)
                dataOutputStream.writeFloatWithLittleEndian(sensors.latitude)
                dataOutputStream.writeFloatWithLittleEndian(sensors.accelerationX)
                dataOutputStream.writeFloatWithLittleEndian(sensors.accelerationY)
                dataOutputStream.writeFloatWithLittleEndian(sensors.accelerationZ)
                dataOutputStream.writeFloatWithLittleEndian(sensors.rotationX)
                dataOutputStream.writeFloatWithLittleEndian(sensors.rotationY)
                dataOutputStream.writeFloatWithLittleEndian(sensors.rotationZ)
                dataOutputStream.writeFloatWithLittleEndian(sensors.gravityX)
                dataOutputStream.writeFloatWithLittleEndian(sensors.gravityY)
                dataOutputStream.writeFloatWithLittleEndian(sensors.gravityZ)

                dataOutputStream.flush()

                delay(100)
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
    val buffer = ByteBuffer.allocate(Float.SIZE_BYTES).order(ByteOrder.LITTLE_ENDIAN)

    buffer.putFloat(value)

    val bytes = buffer.array()

    this.write(bytes)
}

suspend fun readSensors(locationClient: FusedLocationProviderClient? = null): Sensors {
    val location: Location = readLocation(locationClient)

    val sensors = Sensors(
        location.longitude.toFloat(),
        location.latitude.toFloat(),
        0f,
        0f,
        0f,
        0f,
        0f,
        0f,
        0f,
        0f,
        0f,
    )

    Log.d("JuicyNoise", Json.encodeToString(sensors))

    return sensors
}

@SuppressLint("MissingPermission")
suspend fun readLocation(locationClient: FusedLocationProviderClient? = null): Location = suspendCoroutine { continuation ->
    val location = locationClient?.lastLocation ?: throw NullPointerException("Location is null")

    location.addOnCompleteListener {
        if (it.exception == null) {
            continuation.resume(it.result!!)
        } else {
            throw it.exception!!
        }
    }
}

fun ensureAllPermissions(activity: Activity) {
    val context = activity.applicationContext
    val packageManager: PackageManager = context.packageManager
    val packageName: String = context.packageName

    val packageInfo: PackageInfo = packageManager.getPackageInfo(
        packageName,
        PackageManager.GET_PERMISSIONS)

    val ungrantedPermissions = mutableListOf<String>()

    for (permission: String in packageInfo.requestedPermissions) {
        val permissionStatus: Int = packageManager.checkPermission(packageName, permission)

        if (permissionStatus != PackageManager.PERMISSION_GRANTED) {
            ungrantedPermissions.add(permission)
        }
    }

    val requestCode = Random.nextInt()

    ActivityCompat.requestPermissions(
        activity,
        ungrantedPermissions.toTypedArray(),
        requestCode)
}
