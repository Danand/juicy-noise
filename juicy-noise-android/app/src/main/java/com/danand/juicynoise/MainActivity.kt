@file:OptIn(
    ExperimentalMaterial3Api::class,
)

package com.danand.juicynoise

import com.danand.juicynoise.composables.AudioBufferSizeTextField
import com.danand.juicynoise.data.AddressState
import com.danand.juicynoise.data.AudioBufferSize
import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.effects.EffectDelay
import com.danand.juicynoise.signalprocessors.SignalProcessorSensors
import com.danand.juicynoise.ui.theme.JuicyNoiseTheme

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
import android.net.ConnectivityManager
import android.net.ConnectivityManager.NetworkCallback
import android.net.LinkProperties
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.os.Bundle
import android.telephony.SignalStrength
import android.telephony.TelephonyCallback
import android.telephony.TelephonyManager
import android.util.Log

import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.requiredHeight
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.ripple.rememberRipple
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.ButtonDefaults.textButtonColors
import androidx.compose.material3.Checkbox
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Tab
import androidx.compose.material3.TabRow
import androidx.compose.material3.Text
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.core.text.isDigitsOnly

import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import com.google.gson.GsonBuilder

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch

import java.io.DataOutputStream
import java.net.InetSocketAddress
import java.net.Socket
import java.nio.ByteBuffer
import java.nio.ByteOrder

import kotlin.coroutines.resume
import kotlin.coroutines.suspendCoroutine
import kotlin.math.floor
import kotlin.random.Random

class MainActivity : ComponentActivity(), SensorEventListener {
    private val sensorsState = mutableStateOf(Sensors())
    private val isRunningState = mutableStateOf(false)
    private val addressState = createAddressState()
    private val errorState = mutableStateOf<String?>(null)
    private val audioBufferSizeState = mutableStateOf(AudioBufferSize.SIZE_256)
    private val sampleRateState = mutableStateOf(44100)
    private val isShowingSensorsState = mutableStateOf(false)

    private lateinit var sensorManager: SensorManager
    private lateinit var audioOutput: AudioOutput

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

        val telephonyManager = getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager

        val telephonyCallback = object : TelephonyCallback(), TelephonyCallback.SignalStrengthsListener {
            override fun onSignalStrengthsChanged(signalStrength: SignalStrength) {
                sensorsState.value.cellSignalStrength = signalStrength.cellSignalStrengths.maxOf { it.dbm }.toFloat()
            }
        }

        telephonyManager.registerTelephonyCallback(this.mainExecutor, telephonyCallback)

        val connectivityManager = getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager

        val networkCallback = object : NetworkCallback() {
            override fun onCapabilitiesChanged(
                network: Network,
                networkCapabilities: NetworkCapabilities
            ) {
                super.onCapabilitiesChanged(network, networkCapabilities)

                if (networkCapabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)) {
                    sensorsState.value.wifiSignalStrength = networkCapabilities.signalStrength.toFloat()
                }
            }
        }

        val request = NetworkRequest.Builder()
                                    .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
                                    .build()

        connectivityManager.registerNetworkCallback(request, networkCallback)

        sensorManager = getSystemService(Context.SENSOR_SERVICE) as SensorManager

        gyroscope = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE)
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)
        rotationVector = sensorManager.getDefaultSensor(Sensor.TYPE_GAME_ROTATION_VECTOR)
        gravity = sensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY)
        magneticField = sensorManager.getDefaultSensor(Sensor.TYPE_MAGNETIC_FIELD)
        light = sensorManager.getDefaultSensor(Sensor.TYPE_LIGHT)
        pressure = sensorManager.getDefaultSensor(Sensor.TYPE_PRESSURE)
        proximity = sensorManager.getDefaultSensor(Sensor.TYPE_PROXIMITY)

        val signalProcessorSensors = SignalProcessorSensors(sensorsState)
        val effectDelay = EffectDelay(sensorsState)

        audioOutput = AudioOutput(
            arrayOf(
                signalProcessorSensors,
            ),
            arrayOf(
                effectDelay,
            ),
        )

        setContent {
            JuicyNoiseTheme {
                TabScreen(
                    addressState.ip,
                    addressState.port,
                    isRunningState,
                    errorState,
                    sensorsState,
                    audioBufferSizeState,
                    sampleRateState,
                    locationClient,
                    connectivityManager,
                    audioOutput,
                    isShowingSensorsState,
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

        // HACK: Make sensors update in view:
        sensorsState.value = sensorsState.value.clone()

        when (sensorEvent.sensor.type) {
            Sensor.TYPE_GYROSCOPE -> {
                sensorsState.value.angularSpeedX = sensorEvent.values[0]
                sensorsState.value.angularSpeedY = sensorEvent.values[1]
                sensorsState.value.angularSpeedZ = sensorEvent.values[2]
            }
            Sensor.TYPE_ACCELEROMETER -> {
                sensorsState.value.accelerationX = sensorEvent.values[0]
                sensorsState.value.accelerationY = sensorEvent.values[1]
                sensorsState.value.accelerationZ = sensorEvent.values[2]
            }
            Sensor.TYPE_GAME_ROTATION_VECTOR -> {
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

fun findSubnet(connectivityManager: ConnectivityManager): String? {
    val linkProperties: LinkProperties =  connectivityManager.getLinkProperties(connectivityManager.activeNetwork) as LinkProperties
    val linkAddress = linkProperties.linkAddresses.firstOrNull { it?.address?.hostAddress != null } ?: return null

    val addressParts = linkAddress.address.hostAddress!!.split(".")

    if (addressParts.count() > 2) {
        return addressParts.subList(0, 2).joinToString(".")
    }

    return null
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

@Composable
fun TabScreen(
    ipState: MutableState<String>,
    portState: MutableState<UShort>,
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    connectivityManager: ConnectivityManager,
    audioOutput: AudioOutput,
    isShowingSensorsState: MutableState<Boolean>,
) {
    var tabIndex by remember { mutableStateOf(0) }

    val tabs = listOf(
        "Standalone",
        "VST",
    )

    Column(modifier = Modifier.fillMaxWidth()) {
        TabRow(selectedTabIndex = tabIndex) {
            tabs.forEachIndexed { index, title ->
                Tab(text = { Text(title) },
                    selected = tabIndex == index,
                    onClick = { tabIndex = index }
                )
            }
        }
        when (tabIndex) {
            0 -> TabStandalone(
                isRunningState,
                errorState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
                locationClient,
                audioOutput,
                isShowingSensorsState,
            )
            1 -> TabVST(
                ipState,
                portState,
                isRunningState,
                errorState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
                locationClient,
                connectivityManager,
                isShowingSensorsState,
            )
        }
    }
}

@Composable
fun TabStandalone(
    isRunningState: MutableState<Boolean>,
    errorState: MutableState<String?>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    audioOutput: AudioOutput,
    isShowingSensorsState: MutableState<Boolean>,
) {
    Column(
        modifier = Modifier
            .padding(36.dp)
            .verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally,
    ) {
        AudioSettings(
            sampleRateState,
            audioBufferSizeState,
        )

        Spacer(modifier = Modifier.height(24.dp))

        if (isRunningState.value) {
            ButtonStopStandaloneAudio(
                isRunningState,
                audioOutput,
            )
        } else {
            ButtonPlayStandaloneAudio(
                isRunningState,
                sensorsState,
                audioBufferSizeState,
                sampleRateState,
                locationClient,
                audioOutput,
            )
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
        modifier = Modifier
            .padding(36.dp)
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

        AudioSettings(
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

@Composable
fun AudioSettings(
    sampleRateState: MutableState<Int>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
) {
    OutlinedTextField(
        value = sampleRateState.value.toString(),
        onValueChange = {
            sampleRateState.value = it.toInt()
        },
        label = {
            Text("Sample Rate")
        },
        keyboardOptions = KeyboardOptions(
            keyboardType = KeyboardType.Number,
        ),
        modifier = Modifier.fillMaxWidth()
                           .height(72.dp),
    )

    Spacer(modifier = Modifier.height(16.dp))

    AudioBufferSizeTextField(
        selectedBufferSize = audioBufferSizeState.value,
        onItemSelected = { audioBufferSizeState.value = it }
    )
}

@Composable
fun AlertError(
    errorState: MutableState<String?>,
) {
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

@Composable
fun SensorValues(
    sensorsState: MutableState<Sensors>,
    isShowingSensorsState: MutableState<Boolean>
) {
    Spacer(modifier = Modifier.height(24.dp))

    LabelledCheckBox(
        label = "Show sensor values",
        checked = isShowingSensorsState.value,
        onCheckedChange = {
            isShowingSensorsState.value = it
        },
    )

    if (isShowingSensorsState.value) {
        Spacer(modifier = Modifier.height(24.dp))

        val orange = Color.hsv(0f, 0.9843f, 0.9843f)

        enumerateSensors(sensorsState.value).forEach {
            OutlinedTextField(
                value = it.second.toString(),
                onValueChange = { },
                label = {
                    Text(
                        text = it.first,
                        fontFamily = FontFamily.Monospace,
                    )
                },
                modifier = Modifier.fillMaxWidth()
                                   .height(56.dp),
                readOnly = true,
                textStyle = TextStyle(
                    fontFamily = FontFamily.Monospace,
                ),
                colors = TextFieldDefaults.outlinedTextFieldColors(
                    disabledLabelColor = orange,
                    focusedLabelColor = orange,
                    errorLabelColor = orange,
                    unfocusedLabelColor = orange,
                )
            )

            Spacer(modifier = Modifier.height(12.dp))
        }
    }
}

fun enumerateSensors(sensors: Sensors): List<Pair<String, Float>> {
    val result = mutableListOf<Pair<String, Float>>()

    result.add(Pair("longitude", sensors.longitude))
    result.add(Pair("latitude", sensors.latitude))
    result.add(Pair("angularSpeedX", sensors.angularSpeedX))
    result.add(Pair("angularSpeedY", sensors.angularSpeedY))
    result.add(Pair("angularSpeedZ", sensors.angularSpeedZ))
    result.add(Pair("accelerationX", sensors.accelerationX))
    result.add(Pair("accelerationY", sensors.accelerationY))
    result.add(Pair("accelerationZ", sensors.accelerationZ))
    result.add(Pair("rotationX", sensors.rotationX))
    result.add(Pair("rotationY", sensors.rotationY))
    result.add(Pair("rotationZ", sensors.rotationZ))
    result.add(Pair("gravityX", sensors.gravityX))
    result.add(Pair("gravityY", sensors.gravityY))
    result.add(Pair("gravityZ", sensors.gravityZ))
    result.add(Pair("magneticX", sensors.magneticX))
    result.add(Pair("magneticY", sensors.magneticY))
    result.add(Pair("magneticZ", sensors.magneticZ))
    result.add(Pair("light", sensors.light))
    result.add(Pair("pressure", sensors.pressure))
    result.add(Pair("proximity", sensors.proximity))
    result.add(Pair("cellSignalStrength", sensors.cellSignalStrength))
    result.add(Pair("wifiSignalStrength", sensors.wifiSignalStrength))

    return result
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
fun ButtonPlayStandaloneAudio(
    isRunningState: MutableState<Boolean>,
    sensorsState: MutableState<Sensors>,
    audioBufferSizeState: MutableState<AudioBufferSize>,
    sampleRateState: MutableState<Int>,
    locationClient: FusedLocationProviderClient,
    audioOutput: AudioOutput,
) {
    Button(
        onClick = {
            isRunningState.value = true

            runReadingLocation(
                locationClient,
                isRunningState,
                sensorsState,
            )

            audioOutput.play(
                sampleRateState.value,
                audioBufferSizeState.value.value,
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
        enabled = true
    ) {
        Text("Play")
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

@Composable
fun ButtonStopStandaloneAudio(
    isRunning: MutableState<Boolean>,
    audioOutput: AudioOutput,
) {
    Button(
        onClick = {
            isRunning.value = false
            audioOutput.stop()
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
        Text("Stop")
    }
}

@Composable
fun LabelledCheckBox(
    checked: Boolean,
    onCheckedChange: ((Boolean) -> Unit),
    label: String,
    modifier: Modifier = Modifier
) {
    Row(
        verticalAlignment = Alignment.CenterVertically,
        modifier = modifier
            .clip(MaterialTheme.shapes.small)
            .clickable(
                indication = rememberRipple(color = MaterialTheme.colorScheme.primary),
                interactionSource = remember { MutableInteractionSource() },
                onClick = { onCheckedChange(!checked) }
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

fun createAddressState(): AddressState = AddressState(
    ip = mutableStateOf("192.168.0.128"),
    port = mutableStateOf(6660u),
)

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

            if (location != null) {
                sensorsState.value.longitude = location.longitude.toFloat()
                sensorsState.value.latitude = location.latitude.toFloat()
            }

            delay(5000)
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

fun ensureAllPermissions(activity: Activity) {
    val context = activity.applicationContext
    val packageManager: PackageManager = context.packageManager
    val packageName: String = context.packageName

    val packageInfo: PackageInfo = packageManager.getPackageInfo(
        packageName,
        PackageManager.GET_PERMISSIONS,
    )

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
        requestCode,
    )
}
