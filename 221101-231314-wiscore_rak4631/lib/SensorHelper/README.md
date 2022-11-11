# Sensor Helper Library

This library provides functions to initialise and read sensors. It is also a place to collect the associated code needed to do so in the one place: currently includes analog sensors (e.g. battery level), RAK1901, & RAK1906.

The sensors are read and encoded according the specified port number that defines the [sensor](../PortSchema/#sensor-data-payload-encoding) & [port](../PortSchema/#port-definitions) schemas.

## Dependencies

Hardware:

- WisBlock Base & RAK4630
- WisBlock Sensors (RAK1901 & RAK1906) if using (see SensorHelper.h).

Software:

- Arduino.h
- [LoRaWan-RAK4630.h](../../#environment-setup)
- [Logging.h](../Logging/)
- [PortSchema.h](../PortSchema/)
- [SparkFun_SHTC3.h](https://github.com/sparkfun/SparkFun_SHTC3_Arduino_Library) for the RAK1901
- [Adafruit_BME680.h](https://github.com/adafruit/Adafruit_BME680) for the RAK1906

## Usage

Steps:

1. Include SensorHelper.h in the main file.
2. Create a port and set it equal to one of the ports defined in PortSchema.h e.g.: `portSchema port = PORT1;`. See an explanation of [port schemas](../PortSchema/).
3. Check that the correct sensors have been inserted into the base board.
4. Initialise the sensors in `setup()` by passing the created `port` and chosen RAK sensor(s) to `initSensors()`.
5. Start reading the sensors by passing the `port` to `getSensorData()`.
6. (_If sending via LoRaWAN_) Use `portSchema::encodeSensorDataToPayload()` to encode the sensor data to a buffer according to the schema (see [sensor_helper_lorawan_example.cpp](./examples/sensor_helper_lorawan_example.cpp)).

### Simple Example

_Copied from examples\sensor_helper_simple_example.cpp:_

```c++
#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino

#include "Logging.h"      /**< Go here to change the logging level for the entire application. */
#include "PortSchema.h"   /**< Go here to see existing and define new sensor/port schemas. */
#include "SensorHelper.h" /**< Go here to add code for new additional sensors. */

// PORT/SENSOR SELECTION
// The chosen port determines the sensor data included in the payload - see PortSchema.h
// E.g. port 3: battery voltage + temperature
portSchema payload_port = PORT3;

sensorData sensor_data = {};

// Sensor reading interval in [ms] = 5seconds.
const int sensor_reading_interval = 5000;

void setup() {
    // initialise the logging module - function does nothing if APP_LOG_LEVEL in Logging.h = NONE
    initLogging();

    // Init sensors according to payload_port selected in SensorHelper.h
    if (!initSensors(&payload_port, true, false)) { // using RAK1901 for temp
        // error init-ing sensors
        while (true) {
            delay(UINT32_MAX - 1);
        }
    }
}

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
```

## Adding a sensor to the library

_Some recommendations for extending the library to read more sensors..._

Firstly you'll want to follow the steps to [add the sensor to the port schemas](../PortSchema/#new-port-or-sensor-schema-instructions).

### Custom sensor

You'll need to create your own library for the sensor if one does not exist.

I would recommend following a typical object-oriented design pattern (pseudo-code example shown below). Also refer to libraries for sensors from Adafruit (and those in this library) as a guide:

```c++
// An example of a basic object-oriented class declaration
class customSensor {
  public:
    /**
     * @brief Constructor that creates the code object - it should not actually interract with the sensor yet
     * Ideally the sensor should automatically start out in it's lowest power mode until instantiated/read.
     */
    customSensor();

    /**
     * @brief Initialise the sensor, getting it ready for taking readings.
     * @return True if successful, false if not.
     */
    bool init();

    /**
     * @brief Read the sensor to get the value.
     * @return The sensor value (of whatever type it is).
     */
    valuedatatype getValue();

    // Plus additional methods needed to use the sensor
};
```

**NOTE**: The SensorHelper library assumes the sensors will be operated in a blocking mode, meaning the sensor is read when the value is needed, and otherwise is doing nothing.

Then move onto the steps below to insert the sensor library into the correct places.

### Regular off-the-shelf sensor

#### In `SensorHelper.h`:

Include any additional libraries needed for the sensor.

#### In `SensorHelper.cpp`:

Assuming the library for the sensor is object oriented, instantiate it at the top of `SensorHelper.cpp` (along with the other existing sensors). If the sensor is a simple analog sensor use the `AnalogSensor` class with the appropriate ADC parameters.

Then perform the initialisation in `initSensors()`; checking first that it's part of the port_settings.

Then perform the sensor reading in `getSensorData()`, filling in the sensor `data`.

## Issues

Sensors that are plugged in, but not in use by the application, can waste a fair amount of power. Unfortunately some WisBlock sensors do not default to their low power/idle state on power up. If they are not in use by the port then they will not be initialised and put into their idle state manually by the firmware, and hence will waste a lot of power doing nothing. Hence sensors that are not in use by the application should be removed, or you will need to add a special function to manually put them into their respective sleep states.

A warning about the gas resistance sensor on the RAK1906/BME680: a small heater is used to measure the gas resistance, not only does this use a lot of power, but it also can ruin the readings from the other sensors (seeing as the local environment near the heater will not be the same as the general environment - notably temperature). The value also cannot be correlated to any real value in this implementation - see [this](https://forum.rakwireless.com/t/rak7204-gas-resistance-what-should-the-values-tell-me/1716) forum post. Hence the gas resistance can only be used in the context of other gas resistance readings to show a relative change.

## Suggested Next Steps

All of the sensor readings have been set up in their blocking/one-shot modes, if you'd like the modules to perform in other modes their initialisation will need to be modified, and you may need to add an interrupt handler for grabbing the sensor data. Remember that interrupts must be concise, and if you'd like to send the data immediately after receiving it from an interrupt you should not do it in the handler. Instead usage of a Semaphore (or other task/event queue) should be investigated.

The examples provided assume that the same port number will be used for the entire program, however it is simple enough to change which port is used to send data within the application; just be sure to initialise all of the sensors that will be required by the program. An simple example of this may be only sending the battery voltage every hour or day, instead of every payload, as it is really not expected to change very often.

The [Port Definitions table](../PortSchema/#port-definitions) shows that location data can be encoded but the code to operate and read a location sensor does not exist in this library. If you go to SensorHelper.h & .cpp there are some commented out lines that show a pseudocode style example. The encoding of the location data is included to show an example of encoding a mulit-value data point; other examples of multi-value data are inertial data, colour (RGB), etc.
