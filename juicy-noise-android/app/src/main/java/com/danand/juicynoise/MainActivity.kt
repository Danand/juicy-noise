package com.danand.juicynoise

import com.danand.juicynoise.composables.TabScreen
import com.danand.juicynoise.data.AudioBufferSize
import com.danand.juicynoise.data.Sensors
import com.danand.juicynoise.configs.createAddressState
import com.danand.juicynoise.configs.createSettingsState
import com.danand.juicynoise.configs.SAMPLE_RATE_DEFAULT
import com.danand.juicynoise.signalprocessors.SignalProcessorSensors
import com.danand.juicynoise.ui.theme.JuicyNoiseTheme

import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices

import android.app.Activity
import android.content.Context
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.net.ConnectivityManager
import android.net.ConnectivityManager.NetworkCallback
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import android.os.Bundle
import android.telephony.SignalStrength
import android.telephony.TelephonyCallback
import android.telephony.TelephonyManager

import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.runtime.mutableStateOf
import androidx.core.app.ActivityCompat

import kotlin.random.Random

class MainActivity : ComponentActivity(), SensorEventListener {
    private val sensorsState = mutableStateOf(Sensors())
    private val isRunningState = mutableStateOf(false)
    private val addressState = createAddressState()
    private val errorState = mutableStateOf<String?>(null)
    private val audioBufferSizeState = mutableStateOf(AudioBufferSize.SIZE_256)
    private val sampleRateState = mutableStateOf(SAMPLE_RATE_DEFAULT)
    private val isShowingSensorsState = mutableStateOf(false)
    private val isShowingAudioSettings = mutableStateOf(false)
    private val settingsState = createSettingsState()

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

        val signalProcessorSensorsLeft = SignalProcessorSensors(
            sensorsState,
            0,
            settingsState)

        val signalProcessorSensorsRight = SignalProcessorSensors(
            sensorsState,
            1,
            settingsState)

        audioOutput = AudioOutput(
            arrayOf(
                signalProcessorSensorsLeft,
                signalProcessorSensorsRight,
            ),
            arrayOf(
            ),
            settingsState,
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
                    isShowingAudioSettings,
                    settingsState,
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
