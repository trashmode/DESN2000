# Logging Library

A simple logging library to print formatted log messages to Serial.

The log messages use `millis()` for the timestamp and are formatted:

```c++
{H:MM:SS.ms} LOG_LEVEL: message
```

### Log Levels

This library has included four levels of logging messages:

- error --> highest level log
- warn
- info
- debug --> lowest level log

The log level for the entire application is set in Logging.h: `APP_LOG_LEVEL`. For a log message to be printed it must have a `LOG_LEVEL` >= the `APP_LOG_LEVEL`.

To disable all logs set `APP_LOG_LEVEL` in Logging.h to `LOG_LEVEL::NONE`.

## Dependencies

- Arduino.h
- stdarg.h
- Serial interface

## Usage

Steps:

1. Include Logging.h in any file you'd like to print log messages in.
2. Initialise the logging library in `setup()` with `initLogging()`.
3. In Logging.h, set `APP_LOG_LEVEL` to the desired `LOG_LEVEL`.
4. Happy logging :)

### Example

_Copied from examples\simple_logging_example.cpp:_

```c++
#include <Arduino.h>

#include "Logging.h" /**< Go here to change the logging level for the entire application. */

ulong seconds;

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

void loop() {
    delay(1000);
    seconds++;
    // printf style formatting can also be used in log messages
    log(LOG_LEVEL::DEBUG, "Using printf style formatting: \n\t\t     Time elapsed = %lu seconds", seconds);
}
```

#### Resulting Logs:

If `APP_LOG_LEVEL = LOG_LEVEL::DEBUG`:

```
{0:00:02.789} DEBUG: This is a DEBUG level log message.
{0:00:02.789}  INFO: This is an INFO level log message.
{0:00:02.789}  WARN: This is a WARN level log message.
{0:00:02.789} ERROR: This is an ERROR level log message.
{0:00:03.789} DEBUG: Using printf style formatting:
                     Time elapsed = 1 seconds
{0:00:04.789} DEBUG: Using printf style formatting:
                     Time elapsed = 2 seconds
{0:00:05.789} DEBUG: Using printf style formatting:
                     Time elapsed = 3 seconds

...
```

If `APP_LOG_LEVEL = LOG_LEVEL::INFO`:

```
{0:00:02.789}  INFO: This is an INFO level log message.
{0:00:02.789}  WARN: This is a WARN level log message.
{0:00:02.789} ERROR: This is an ERROR level log message.
```

If `APP_LOG_LEVEL = LOG_LEVEL::WARN`:

```
{0:00:02.789}  WARN: This is a WARN level log message.
{0:00:02.789} ERROR: This is an ERROR level log message.
```

If `APP_LOG_LEVEL = LOG_LEVEL::ERROR`:

```
{0:00:02.789} ERROR: This is an ERROR level log message.
```

If `APP_LOG_LEVEL = LOG_LEVEL::NONE`:

```

```

## Issues

Serial is power intensive so logs should be disabled when not in use and especially if trying to measure power performance.

Serial is also slow so sometimes a log may not appear if a `delay()` is not added before the program progresses (especially if going into any blocking functions). This also means logs in interrupt functions can cause issues and should be avoided unless absolutely necessary.

## Suggested Next Steps

Currently the logs are directed to Serial. If you'd like to direct them to a different location (e.g. EEPROM, SD card, etc.), then you will need to edit/replace two functions in `logging.cpp`: `initSerial()` & `printLog()`.
