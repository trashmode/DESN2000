#include "logging.h"

// forward declarations
void formatTimestamp(unsigned long timestamp, char *buffer, int buffer_len);
void printLog(char *log);
void initSerial(void);

void initLogging(void) {
    if (APP_LOG_LEVEL == LOG_LEVEL::NONE) {
        // do nothing
        return;
    }
    // change which function is called here if logging to a different location e.g. EEPROM, SD card, etc.
    initSerial();
}

void log(LOG_LEVEL level, const char *format, ...) {
    if ((level > APP_LOG_LEVEL) || (level == LOG_LEVEL::NONE)) {
        // do nothing
        return;
    }

    char log_message[MAX_LOG_LENGTH] = {};

    // using stdarg.h to grab the additional arguments (the ...) and use them in the vsnprintf to insert into format
    va_list args;
    va_start(args, format);
    vsnprintf(log_message, sizeof(log_message), format, args);
    va_end(args);

    char log_level_prefix[6] = {};
    switch (level) {
        case LOG_LEVEL::DEBUG:
            strcpy(log_level_prefix, "DEBUG");
            break;
        case LOG_LEVEL::INFO:
            strcpy(log_level_prefix, " INFO"); // extra space is to pad the string to the same length
            break;
        case LOG_LEVEL::WARN:
            strcpy(log_level_prefix, " WARN"); // extra space is to pad the string to the same length
            break;
        case LOG_LEVEL::ERROR:
            strcpy(log_level_prefix, "ERROR");
            break;
        default:
            // no level?
            break;
    }

    char timestamp[40] = {};
    formatTimestamp(millis(), timestamp, sizeof(timestamp));

    // assemble formatted log: "{timestamp} LOG_LEVEL: message"
    char printable_log[MAX_LOG_LENGTH] = {};
    snprintf(printable_log, sizeof(printable_log), "{%s} %s: %s", timestamp, log_level_prefix, log_message);

    printLog(printable_log);
}

/**
 * @brief Initialise Serial.
 * Flashes the LED_BUILTIN while waiting for Serial.
 */
void initSerial(void) {
    // Initialize the board LEDs
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_CONN, OUTPUT);

    // Initialize Serial for debug output
    Serial.begin(115200);

    // Wait for upto 5 seconds for serial to connect
    time_t serial_timeout = millis();
    // Toggle LED_BUILTIN while waiting for Serial
    digitalWrite(LED_BUILTIN, LOW);
    while (!Serial) {
        if ((millis() - serial_timeout) < 5000) {
            delay(100);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        } else {
            // stop waiting
            break;
        }
    }
    // make sure LED is now off
    digitalWrite(LED_BUILTIN, LOW);
}

/**
 * @brief Format the given timestamp into "HH:MM:SS.ms".
 * @param timestamp Timestamp to format.
 * @param buffer Buffer to place formatted timestamp into.
 * @param buffer_len Length of the buffer.
 */
void formatTimestamp(unsigned long timestamp, char *buffer, int buffer_len) {
    unsigned long HH, MM, SS, ms;
    HH = timestamp / MS_IN_HOUR;
    MM = (timestamp % MS_IN_HOUR) / MS_IN_MINUTE;
    SS = (timestamp % MS_IN_MINUTE) / MS_IN_SECOND;
    ms = timestamp % MS_IN_SECOND;
    // place formatted timestamp into given buffer
    snprintf(buffer, buffer_len, "%lu:%02lu:%02lu.%03lu", HH, MM, SS, ms);
}

/**
 * @brief Prints the formatted log.
 * Change this function to alter the end location of the logs:
 * e.g. EEPROM, SD card, etc.
 * @param log The formatted log message to print.
 */
void printLog(char *log) {
    Serial.println(log);
}
