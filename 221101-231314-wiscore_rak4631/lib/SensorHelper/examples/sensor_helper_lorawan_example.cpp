/**
 * @file main.cpp
 * @author Kalina Knight
 * @brief An example of using the Sensor Helper library with LoRaWAN.
 * Make sure to match the chosen payload_port with the onboard sensors available.
 * e.g. portSchema payload_port = PORT3; requires a RAK1901 or RAK1906 to acquire the temperature.
 * You also need to indicate which sensor to use in SensorHelper.h by setting the USE_RAK1901, etc. to true.
 *
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino

#include "LoRaWAN_functs.h" /**< Go here to provide the OTAA keys & change the LoRaWAN settings. */
#include "Logging.h"        /**< Go here to change the logging level for the entire application. */
#include "OTAA_keys.h"      /**< Go here to set the OTAA keys (See LoRaWAN_functs README). */
#include "PortSchema.h"     /**< Go here to see existing and define new sensor/port schemas. */
#include "SensorHelper.h"   /**< Go here to add code for new additional sensors. */

// PORT/SENSOR SELECTION
// The chosen port determines the sensor data included in the payload - see PortSchema.h
// E.g. port 3: battery voltage + temperature
// This example uses the RAK1901 for temp - change initSensors to switch to the 1906
portSchema payload_port = PORT3;

// Sensor reading interval in [ms] = 30 seconds.
const int sensor_reading_interval = 30000;

// PAYLOAD ENCODING
uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE] = {};                /**< Buffer that payload data is placed in. */
lmh_app_data_t lorawan_payload = { payload_buffer, 0, 0, 0, 0 }; /**< Struct that passes the payload buffer and relevant
                                                                    params for a LoRaWAN frame. */
// forward declaration
void fillPayload(void);

/**
 * @brief Setup code runs once on reset/startup.
 */
void setup() {
    // initialise the logging module - function does nothing if APP_LOG_LEVEL in Logging.h = NONE
    initLogging();
    log(LOG_LEVEL::INFO,
        "\n========================================"
        "\nWelcome to Sensor Helper LoRaWAN Example"
        "\n========================================");

    // Init sensors according to payload_port selected
    if (!initSensors(&payload_port, true, false)) {
        return;
    }

    // Init LoRaWAN
    if (!initLoRaWAN(OTAA_KEY_APP_EUI, OTAA_KEY_DEV_EUI, OTAA_KEY_APP_KEY)) {
        return;
    }

    // Attempt to join the network
    startLoRaWANJoinProcedure();
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    // every sensor_reading_interval ms check if connected and then send sensor payload
    delay(sensor_reading_interval);
    if (isLoRaWANConnected()) {
        log(LOG_LEVEL::DEBUG, "Send payload");
        // fill lora data buffer
        fillPayload();
        // send data
        sendLoRaWANFrame(&lorawan_payload);
    } else {
        log(LOG_LEVEL::DEBUG, "LoRaWAN not connected. Try again later.");
    }
}

/**
 * @brief Gets the sensor data, then fills payload_buffer with the encoded data ready for sending via LoRaWAN.
 * Follows the portSchema specified in PortSchema.h.
 */
void fillPayload(void) {
    // get the sensor data
    sensorData sensor_data = {};
    sensor_data = getSensorData(&payload_port);

    log(LOG_LEVEL::INFO, "b: %.2f %% | t: %.2f C | h: %.2f %% | p: %lu Pa | g: %lu | l: %.5f, %.5f",
        sensor_data.battery_mv.value, sensor_data.temperature.value, sensor_data.humidity.value, sensor_data.pressure.value,
        sensor_data.gas_resist.value, sensor_data.location.latitude, sensor_data.location.longitude);

    // clear the buffer
    memset(payload_buffer, 0, sizeof(payload_buffer));
    lorawan_payload.buffsize = 0;
    lorawan_payload.port = payload_port.port_number;

    // encode the sensor data to lorawan_payload
    lorawan_payload.buffsize = payload_port.encodeSensorDataToPayload(&sensor_data, payload_buffer);
}
