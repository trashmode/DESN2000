#pragma once
/**
 * @file Logging.h
 * @author Kalina Knight
 * @brief Some basic functions for logging.
 * Change the APP_LOG_LEVEL to turn log messages on and off.
 * Currently the logs are directed to Serial. If you'd like to direct them to a different location (e.g. EEPROM, SD
 * card, etc.), then you will need to edit/replace two functions in logging.cpp: initSerial() & printLog().
 * @version 0.1
 * @date 2021-08-17
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Arduino.h>
#include <stdarg.h>

enum class LOG_LEVEL {
    NONE = 0,  /**< Disable logging. No messages are logged. */
    ERROR = 1, /**< Only ERROR level messages are logged. */
    WARN = 2,  /**< ERROR & WARN level messages are logged. */
    INFO = 3,  /**< ERROR, WARN & INFO level messages are logged. */
    DEBUG = 4  /**< ERROR, WARN, INFO & DEBUG level messages are logged. */
};

// Set the logging level for the entire application here:
#define APP_LOG_LEVEL LOG_LEVEL::DEBUG

// I'm unsure what the max is for Serial, but 200 characters seems like plenty
#define MAX_LOG_LENGTH 200

// Milliseconds in X used in formatTimestamp()
#define MS_IN_HOUR   (60 * MS_IN_MINUTE)
#define MS_IN_MINUTE (60 * MS_IN_SECOND)
#define MS_IN_SECOND 1000

/**
 * @brief Initialises location where logs are sent.
 */
void initLogging(void);

/**
 * @brief Formats and logs the message if it is of level >= APP_LOG_LEVEL.
 * Logs are formatted with a timestamp and the level: "{timestamp} LOG_LEVEL: message".
 * The arguments format and ... work just like printf() statements.
 * @param level The level of the log message. See enum LOG_LEVEL.
 * @param format Print format for the message.
 * @param ... (Optional) Any additional arguments for the format.
 */
void log(LOG_LEVEL level, const char *format, ...);
