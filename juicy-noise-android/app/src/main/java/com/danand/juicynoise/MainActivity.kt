@file:OptIn(
    ExperimentalMaterial3Api::class,
)

package com.danand.juicynoise

import android.annotation.SuppressLint
import android.app.Activity
import android.content.Context
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.location.Location
import android.os.Bundle
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
import java.io.DataOutputStream
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder
import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine
import kotlin.random.Random

class MainActivity : ComponentActivity(), SensorEventListener {
    private val sensorsState = mutableStateOf(Sensors())
    private val isRunningState = mutableStateOf(false)

    private lateinit var sensorManager: SensorManager

    private var gyroscope: Sensor? = null
    private var accelerometer: Sensor? = null
    private var rotationVector: Sensor? = null
    private var gravity: Sensor? = null
    private var magneticField: Sensor? = null
    private var light: Sensor? = null
    private var pressure: Sensor? = null
    private var proximity: Sensor? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        ensureAllPermissions(this)

        val locationClient: FusedLocationProviderClient = LocationServices.getFusedLocationProviderClient(this)

        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager

        gyroscope = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE)
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)
        rotationVector = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR)
        gravity = sensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY)
        magneticField = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD)
        light = sensorManager.getDefaultSensor(Sensor.TYPE_LIGHT)
        pressure = sensorManager.getDefaultSensor(Sensor.TYPE_PRESSURE)
        proximity = sensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY)

        setContent {
            JuicyNoiseTheme() {
                val addressState = remember { createDefaultState() }
                val errorState = remember { mutableStateOf<String?>(null) }

                ColumnMain(
                    addressState.ip,
                    addressState.port,
                    isRunningState,
                    errorState,
                    sensorsState,
                    locationClient,
                )
            }
        }
    }

    override fun onResume() {
        super.onResume()

        sensorManager.registerListener(this, gyroscope, SensorManager.SENSOR_DELAY_NORMAL)
        sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL)
        sensorManager.registerListener(this, rotationVector, SensorManager.SENSOR_DELAY_NORMAL)
        sensorManager.registerListener(this, gravity, SensorManager.SENSOR_DELAY_NORMAL)
        sensorManager.registerListener(this, magneticField, SensorManager.SENSOR_DELAY_NORMAL)
        sensorManager.registerListener(this, light, SensorManager.SENSOR_DELAY_NORMAL)
        sensorManager.registerListener(this, pressure, SensorManager.SENSOR_DELAY_NORMAL)
        sensorManager.registerListener(this, proximity, SensorManager.SENSOR_DELAY_NORMAL)
    }

    override fun onPause() {
        super.onPause()

        sensorManager.unregisterListener(this, gyroscope)
        sensorManager.unregisterListener(this, accelerometer)
        sensorManager.unregisterListener(this, rotationVector)
        sensorManager.unregisterListener(this, gravity)
        sensorManager.unregisterListener(this, magneticField)
        sensorManager.unregisterListener(this, light)
        sensorManager.unregisterListener(this, pressure)
        sensorManager.unregisterListener(this, proximity)
    }

    override fun onSensorChanged(sensorEvent: SensorEvent?) {
        if (sensorEvent == null) {
            return
        }

        when (sensorEvent.sensor.type) {
            Sensor.TYPE_GYROSCOPE-> {
                sensorsState.value.angularSpeedX = sensorEvent.values[0]
                sensorsState.value.angularSpeedY = sensorEvent.values[1]
                sensorsState.value.angularSpeedZ = sensorEvent.values[2]
            }
            Sensor.TYPE_ACCELEROMETER -> {
                sensorsState.value.accelerationX = sensorEvent.values[0]
                sensorsState.value.accelerationY = sensorEvent.values[1]
                sensorsState.value.accelerationZ = sensorEvent.values[2]
            }
            Sensor.TYPE_ROTATION_VECTOR -> {
                sensorsState.value.rotationX = sensorEvent.values[0]
                sensorsState.value.rotationY = sensorEvent.values[1]
                sensorsState.value.rotationZ = sensorEvent.values[2]
            }
            Sensor.TYPE_GRAVITY -> {
                sensorsState.value.gravityX = sensorEvent.values[0]
                sensorsState.value.gravityY = sensorEvent.values[1]
                sensorsState.value.gravityZ = sensorEvent.values[2]
            }
            Sensor.TYPE_MAGNETIC_FIELD -> {
                sensorsState.value.magneticX = sensorEvent.values[0]
                sensorsState.value.magneticY = sensorEvent.values[1]
                sensorsState.value.magneticZ = sensorEvent.values[2]
            }
            Sensor.TYPE_LIGHT -> {
                sensorsState.value.light = sensorEvent.values[0]
            }
            Sensor.TYPE_PRESSURE -> {
                sensorsState.value.pressure = sensorEvent.values[0]
            }
            Sensor.TYPE_PROXIMITY -> {
                sensorsState.value.proximity = sensorEvent.values[0]
            }
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) { }
}

@Composable
fun ColumnMain(
    ipState: MutableState<String>,
    portState: MutableState<String>,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    locationClient: FusedLocationProviderClient,
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
                sensorsState,
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
    sensorsState: MutableState<Sensors>,
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

fun createDefaultState(): AddressState = AddressState(
    ip = mutableStateOf("192.168.0.1"),
    port = mutableStateOf("6660"),
)

fun startSendingSensors(
    ip: String,
    port: UShort,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
) {
    val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    scope.launch(Dispatchers.IO) {
        try {
            val socket = Socket(ip, port.toInt())
            val socketStream = socket.getOutputStream()
            val dataOutputStream = DataOutputStream(socketStream)

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

fun runReadingLocation(
    locationClient: FusedLocationProviderClient,
    isRunningState: MutableState<Boolean>,
    sensorsState: MutableState<Sensors>,
) {
    val scope = CoroutineScope(Dispatchers.Main + SupervisorJob())

    scope.launch(Dispatchers.Main) {
        while (isRunningState.value) {
            val location = readLocation(locationClient)

            sensorsState.value.longitude = location.longitude.toFloat()
            sensorsState.value.latitude = location.latitude.toFloat()

            delay(100)
        }
    }
}

@SuppressLint("MissingPermission")
suspend fun readLocation(locationClient: FusedLocationProviderClient): Location = suspendCoroutine { continuation ->
    locationClient.lastLocation.addOnCompleteListener {
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

    val requestCode = Random.nextInt(1, Int.MAX_VALUE)

    ActivityCompat.requestPermissions(
        activity,
        ungrantedPermissions.toTypedArray(),
        requestCode)
}
