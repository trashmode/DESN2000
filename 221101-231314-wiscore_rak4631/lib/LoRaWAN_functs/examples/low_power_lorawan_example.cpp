/**
 * @file low_power_lorawan_example.cpp
 * @author Kalina Knight
 * @brief A low power example of the LoRaWAN functions that uses the FreeRTOS Semaphores for 'sleeping'.
 * See the README of combined_lib_example.cpp for further explanation of Semaphores.
 * @version 0.1
 * @date 2021-08-27
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino

#include "LoRaWAN_functs.h" /**< Go here to provide the OTAA keys & change the LoRaWAN settings. */
#include "Logging.h"        /**< Go here to change the logging level for the entire application. */
#include "OTAA_keys.h"      /**< Go here to set the OTAA keys (See README). */

// APP TIMER
const int lorawan_app_interval = 30000; /**< App payloadTimer interval value in [ms] = 30 seconds. */
SoftwareTimer payloadTimer;             /**< payloadTimer to wakeup task and send payload. */
// forward declarations
static void appTimerInit(void);
static void appTimerTimeoutHandler(TimerHandle_t unused);

// POWER SAVING - see README for further details on Semaphores & low power mode
// TODO: not sure about pdFalse
static SemaphoreHandle_t semaphore_handle = NULL; /**< Semaphore used by events to wake up loop task. */
enum class EVENT_TASK {
    SLEEP,        /**< Use semaphore take to "sleep" in a low power state. */
    SEND_PAYLOAD, /**< Send a sensor reading payload. */
};
static EVENT_TASK current_task = EVENT_TASK::SLEEP; /**< Current task of the device, similar to a finite state machine
                                                       design. Used in loop() to switch between tasks. */

// PAYLOAD
uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE] = "Hello!"; /**< Buffer that payload data is placed in. */
lmh_app_data_t lorawan_payload = { payload_buffer, strlen("Hello!"), 1, 0, 0 }; /**< Struct that passes the payload
                                                                                   buffer and relevant params for a
                                                                                   LoRaWAN frame. */

/**
 * @brief Setup code runs once on reset/startup.
 */
void setup() {
    // initialise the logging module - function does nothing if APP_LOG_LEVEL in Logging.h = NONE
    initLogging();
    log(LOG_LEVEL::INFO,
        "\n===================================="
        "\nWelcome to Low Power LoRaWAN Example"
        "\n====================================");

    // Create the semaphore that will enable low power 'sleep'
    semaphore_handle = xSemaphoreCreateBinary();

    // Init payloadTimer
    appTimerInit();

    // Init LoRaWAN passing in the timer and desired TX power setting
    if (!initLoRaWAN(&payloadTimer, OTAA_KEY_APP_EUI, OTAA_KEY_DEV_EUI, OTAA_KEY_APP_KEY, TX_POWER_0)) {
        delay(1000);
        return;
    }

    // Attempt to join the network
    startLoRaWANJoinProcedure();

    // Go to 'sleep' now that setup is complete until an event task is triggered
    current_task = EVENT_TASK::SLEEP;
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    switch (current_task) {
        case EVENT_TASK::SLEEP:
            // Sleep until we are woken up by an event
            log(LOG_LEVEL::DEBUG, "Semaphore sleep");
            // This function call puts the device to 'sleep' in low power mode.
            // The semaphore can only be taken once given in appTimerTimeoutHandler() (or another function).
            // It will wait (up to portMAX_DELAY ticks) for the semaphore_handle semaphore to be given.
            xSemaphoreTake(semaphore_handle, portMAX_DELAY);
            // This point is only reached if the semaphore was able to be taken or the function timed out.
            // If the semaphore was able to be taken, then the current_task should have been set before giving the
            // semaphore. If xSemaphoreTake() timed out then the current_task has not changed and the switch-case will
            // come back to EVENT_TASK::SLEEP and sleep again.
            break;

        case EVENT_TASK::SEND_PAYLOAD:
            // send sendLoRaWANFrame will do nothing if not connected
            log(LOG_LEVEL::DEBUG, "Send payload");
            sendLoRaWANFrame(&lorawan_payload);
            // go back to 'sleep'
            current_task = EVENT_TASK::SLEEP;
            break;

        default:
            // just in case there's an unknown current_task, go to 'sleep'
            current_task = EVENT_TASK::SLEEP;
            break;
    }
}

/**
 * @brief Function for the payloadTimer initialization.
 * Initializes the payloadTimer as repeating with lorawan_app_interval timeout.
 */
void appTimerInit(void) {
    log(LOG_LEVEL::DEBUG, "Initialising timer...");
    payloadTimer.begin(lorawan_app_interval, appTimerTimeoutHandler);
}

/**
 * @brief Function for handling payloadTimer timeout event.
 * Sets the current_task to SEND_PAYLOAD and then 'wakes' the device by giving the semaphore so then it can be taken in
 * loop(), and the switch case will move to the new current_task.
 */
void appTimerTimeoutHandler(TimerHandle_t unused) {
    current_task = EVENT_TASK::SEND_PAYLOAD;
    // Give the semaphore, so the loop task can take it and wake up
    xSemaphoreGiveFromISR(semaphore_handle, pdFALSE);
}