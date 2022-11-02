/**
 * @file main.cpp
 * @author Kalina Knight
 * @brief A simple logging library example.
 *
 * @details The logging library only needs to be initialised and then you can start logging.
 * To change the logging level go to Logging.h and set APP_LOG_LEVEL to the desired LOG_LEVEL.
 *
 * @version 0.1
 * @date 2021-08-26
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Arduino.h>

#include "Logging.h" /**< Go here to change the logging level for the entire application. */

ulong seconds;

/**
 * @brief Setup code runs once on reset/startup.
 */
void setup() {
    // initialise the logging module
    initLogging();

    // now start logging :)
    log(LOG_LEVEL::DEBUG, "This is a DEBUG level log message.");
    log(LOG_LEVEL::INFO, "This is an INFO level log message.");
    log(LOG_LEVEL::WARN, "This is a WARN level log message.");
    log(LOG_LEVEL::ERROR, "This is an ERROR level log message.");
    // APP_LOG_LEVEL = LOG_LEVEL::NONE disables logging.
    log(LOG_LEVEL::NONE, "This message will never print as the level doesn't really make sense.");

    seconds = 0;
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    delay(1000);
    seconds++;
    // printf style formatting can also be used in log messages
    log(LOG_LEVEL::DEBUG, "Using printf style formatting: \n\t\t     Time elapsed = %lu seconds", seconds);
}
