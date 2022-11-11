/**
 * @file main.cpp
 * @author Kalina Knight
 * @brief A simple example of using the Sensor Helper library.
 * Make sure to match the chosen payload_port with the onboard sensors available.
 * e.g. portSchema payload_port = PORT3; requires a RAK1901 or RAK1903 to acquire the temperature.
 * You also need to indicate which sensor to use in SensorHelper.h by setting the USE_RAK1901, etc. to true.
 *
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino

#include "Logging.h"      /**< Go here to change the logging level for the entire application. */
#include "PortSchema.h"   /**< Go here to see existing and define new sensor/port schemas. */
#include "SensorHelper.h" /**< Go here to add code for new additional sensors. */

// PORT/SENSOR SELECTION
// The chosen port determines the sensor data included in the payload - see PortSchema.h
// E.g. port 3: battery voltage + temperature
// This example uses the RAK1901 for temp - change initSensors to switch to the 1906
portSchema payload_port = PORT3;

sensorData sensor_data = {};

// Sensor reading interval in [ms] = 5seconds.
const int sensor_reading_interval = 5000;

/**
 * @brief Setup code runs once on reset/startup.
 */
void setup() {
    // initialise the logging module - function does nothing if APP_LOG_LEVEL in Logging.h = NONE
    initLogging();
    log(LOG_LEVEL::INFO,
        "\n======================================="
        "\nWelcome to Simple Sensor Helper Example"
        "\n=======================================");

    // Init sensors according to payload_port selected
    if (!initSensors(&payload_port, true, false)) {
        // error init-ing sensors
        while (true) {
            delay(UINT32_MAX - 1);
        }
    }
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    // read sensors every sensor_reading_interval ms
    delay(sensor_reading_interval);

    // reset sensor data and encoded buffer
    memset(&sensor_data, 0, sizeof(sensor_data));

    // get the sensor data
    sensor_data = getSensorData(&payload_port);

    log(LOG_LEVEL::INFO, "b: %.2f %% | t: %.2f C | h: %.2f %% | p: %lu Pa | g: %lu | l: %.5f, %.5f",
        sensor_data.battery_mv.value, sensor_data.temperature.value, sensor_data.humidity.value, sensor_data.pressure.value,
        sensor_data.gas_resist.value, sensor_data.location.latitude, sensor_data.location.longitude);
}
