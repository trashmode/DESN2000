#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino
#include "Logging.h" /**< Go here to change the logging level for the entire application. */
// #include <Arduino.h>

static const uint8_t SENSOR_SWITCH_PIN = WB_IO2;

void Sensor_on () {
    pinMode(SENSOR_SWITCH_PIN, OUTPUT);
    digitalWrite(SENSOR_SWITCH_PIN, HIGH);
    log(LOG_LEVEL::INFO, "Sensor powered on.");
}

void Sensor_off() {
    digitalWrite(SENSOR_SWITCH_PIN, LOW);
    log(LOG_LEVEL::INFO, "Sensor powered off.");
}


