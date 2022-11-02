/**
 * @file low_power_lorawan_example.cpp
 * @author Kalina Knight
 * @brief A simple example of the LoRaWAN functions.
 * @version 0.1
 * @date 2021-08-27
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino
// #include "Logging.h" /**< Go here to change the logging level for the entire application. */
#include "LoRaWAN_functs.h" /**< Go here to provide the OTAA keys & change the LoRaWAN settings. */
#include "Logging.h"        /**< Go here to change the logging level for the entire application. */
#include "OTAA_keys.h"      /**< Go here to set the OTAA keys (See README). */

// App payload interval value in [ms] = 30 seconds.
const int lorawan_app_interval = 30000;

// Buffer that payload data is placed in.
uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE] = "Hello!";

// Struct that passes the payload data for a LoRaWAN frame.
lmh_app_data_t lorawan_payload = {
    payload_buffer, strlen("Hello!"), 1, 0, 0,
};

/**
 * @brief Setup code runs once on reset/startup.
 */
void setup() {
    // initialise the logging module - function does nothing if APP_LOG_LEVEL in Logging.h = NONE
    initLogging();
    log(LOG_LEVEL::INFO,
        "\n================================="
        "\nWelcome to Simple LoRaWAN Example"
        "\n=================================");

    // Init LoRaWAN
    if (!initLoRaWAN(OTAA_KEY_APP_EUI, OTAA_KEY_DEV_EUI, OTAA_KEY_APP_KEY)) {
        delay(1000);
        return;
    }

    // Attempt to join the network
    startLoRaWANJoinProcedure();
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    // every lorawan_app_interval milliseconds check if the device is connected
    delay(lorawan_app_interval);
    if (isLoRaWANConnected()) {
        log(LOG_LEVEL::DEBUG, "Send payload");
        // send sendLoRaWANFrame will do nothing if not connected anyway, but it's best practice to check
        sendLoRaWANFrame(&lorawan_payload);
    } else {
        // else log that it's not connected
        log(LOG_LEVEL::DEBUG, "LoRaWAN not connected. Try again later.");
    }
    log(LOG_LEVEL::DEBUG, "Looped");
}
