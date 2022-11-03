/**
 * @file main.cpp
 * @author Kalina Knight
 * @brief An example of using the Port Schema library with LoRaWAN.
 * This example collects and sends fake sensor data.
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

// PORT/SENSOR SELECTION
// The chosen port determines the sensor data included in the payload - see PortSchema.h
// E.g. port 3: battery voltage + temperature
#define PORT_LIST_LENGTH 19
portSchema port_list[PORT_LIST_LENGTH] = {
    PORT1,  PORT2,  PORT3,  PORT4,  PORT5,  PORT6,  PORT7,  PORT8,  PORT9,  PORT50,
    PORT51, PORT52, PORT53, PORT54, PORT55, PORT56, PORT57, PORT58, PORT59,
};
uint8_t p;

// fill with fake data, making sure to set the validity flag to true
sensorData sensor_data = { 1, true, 2, true, 3, true, 4, true, 5, true, 6, 7, true };

// Sensor reading interval in [ms] = 30 seconds.
const int encoding_interval = 30000;

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
        "\n======================================"
        "\nWelcome to Port Schema LoRaWAN Example"
        "\n======================================");

    // Init LoRaWAN
    if (!initLoRaWAN(OTAA_KEY_APP_EUI, OTAA_KEY_DEV_EUI, OTAA_KEY_APP_KEY)) {
        return;
    }

    // Attempt to join the network
    startLoRaWANJoinProcedure();

    p = 0;
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    // every encoding_interval ms check if connected and then send sensor payload
    delay(encoding_interval);
    if (isLoRaWANConnected()) {
        log(LOG_LEVEL::DEBUG, "Send payload");
        if (p == 0) {
            // log sensor data
            log(LOG_LEVEL::INFO,
                "Sensor Data: {b: %.2f mV | t: %.2f C | h: %.2f %% | p: %lu Pa | g: %lu | l: %.5f, %.5f}",
                sensor_data.battery_mv.value, sensor_data.temperature.value, sensor_data.humidity.value,
                sensor_data.pressure.value, sensor_data.gas_resist.value, sensor_data.location.latitude,
                sensor_data.location.longitude);
        } else if (p >= PORT_LIST_LENGTH) {
            p = 0;
        }
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
    // cycle over port list
    portSchema payload_port = port_list[p++];

    // reset the payload
    memset(payload_buffer, 0, sizeof(payload_buffer));
    lorawan_payload.buffsize = 0;
    lorawan_payload.port = payload_port.port_number;

    // encode the sensor data to lorawan_payload
    lorawan_payload.buffsize = payload_port.encodeSensorDataToPayload(&sensor_data, payload_buffer);

    // log the encoded bytes
    char encoded_payload_bytes[3 * PAYLOAD_BUFFER_SIZE] = {};
    for (int b = 0; b < lorawan_payload.buffsize; b++) {
        snprintf(encoded_payload_bytes, sizeof(encoded_payload_bytes), "%s%02X ", encoded_payload_bytes, payload_buffer[b]);
    }
    log(LOG_LEVEL::INFO, "Port: %2.d | Payload: %s", lorawan_payload.port, encoded_payload_bytes);
}
