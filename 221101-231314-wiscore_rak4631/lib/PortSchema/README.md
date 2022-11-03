# Port Schema Library

This library provides an implementation of a port schema that properly utilises the FPort component of LoRaWAN message. The port field (FPort) in a LoRaWAN message defines what it contains. The LoRaWAN specification reserves FPorts 0 and 224-255, leaving the remaining 1-223 FPorts open for user applications to implement.

The schema defines which sensors data is included in a LoRaWAN payload and how to encode them into byte format.

## Dependencies

Hardware:

- WisBlock Base & RAK4630

Software:

- Arduino.h
- [LoRaWan-RAK4630.h](../../#environment-setup)
- [Logging.h](../Logging/)

## Usage

Steps:

1. Include PortSchema.h in the main file.
2. Create a port and set it equal to one of the ports defined in PortSchema.h e.g.: `portSchema port = PORT1;`. See explanation of [port schemas](#lorawan-ports) below.
3. Fill a `sensorData` struct with data and pass it to the port to encode with `portSchema::encodeSensorDataToPayload()`

### Simple Example

_Copied from examples\port_schema_simple_example.cpp:_

```c++
#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino

#include "Logging.h"    /**< Go here to change the logging level for the entire application. */
#include "PortSchema.h" /**< Go here to see existing and define new sensor/port schemas. */

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

// Sensor reading interval in [ms] = 2 seconds.
const int encoding_interval = 2000;

// PAYLOAD ENCODING
#define PAYLOAD_BUFFER_SIZE 64
uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE] = {};                /**< Buffer that payload data is placed in. */
lmh_app_data_t lorawan_payload = { payload_buffer, 0, 0, 0, 0 }; /**< Struct that passes the payload buffer and relevant
                                                                    params for a LoRaWAN frame. */

void setup() {
    // initialise the logging module - function does nothing if APP_LOG_LEVEL in Logging.h = NONE
    initLogging();

    // log sensor data
    log(LOG_LEVEL::INFO, "Sensor Data: {b: %.2f mV | t: %.2f C | h: %.2f %% | p: %lu Pa | g: %lu | l: %.5f, %.5f}",
        sensor_data.battery_mv.value, sensor_data.temperature.value, sensor_data.humidity.value, sensor_data.pressure.value,
        sensor_data.gas_resist.value, sensor_data.location.latitude, sensor_data.location.longitude);

    p = 0;
}

/**
 * @brief Loop code runs repeated after setup().
 */
void loop() {
    delay(encoding_interval);
    if (p < PORT_LIST_LENGTH) {
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
}
```

## LoRaWAN Ports

Port numbers are used by LoRaWAN to instruct the decoder how to interpret the payload. They are sent with all payloads, and so should not be wasted.

Each port has a different set of sensor data included in the payload packet - always in the same order.
The schema has been designed with a couple of rules:

- Ports numbered 223 onwards are reserved in the LoRaWAN spec.
- Odd numbered ports replicate the format of the previous port (port_number - 1) with battery voltage added to the start of payload.
- Ports numbered 50 onwards replicate the format of ports 1 - 49 with location added to the payload.
- Ports numbered 200-222 should be used for any custom system/control messages - although this has not currently been defined.

### Port Definitions

Currently 19 ports have been designed and assigned a port number (PN) (see [portSchema](#portschema) for how they're defined in code):

| Port Number (PN) |  Battery Voltage   |    Temperature     | Relative Humidity  |    Air Pressure    |   Gas Resistance   |      Location      | Total Length |
| :--------------: | :----------------: | :----------------: | :----------------: | :----------------: | :----------------: | :----------------: | :----------: |
|        1         | :heavy_check_mark: |         -          |         -          |         -          |         -          |         -          |      2       |
|        2         |         -          | :heavy_check_mark: |         -          |         -          |         -          |         -          |      2       |
|        3         | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          |         -          |         -          |      4       |
|        4         |         -          | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          |         -          |      3       |
|        5         | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          |         -          |      5       |
|        6         |         -          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          |      7       |
|        7         | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          |      9       |
|        8         |         -          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          |      11      |
|        9         | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          |      13      |
|        50        |         -          |         -          |         -          |         -          |         -          | :heavy_check_mark: |      8       |
|        51        | :heavy_check_mark: |         -          |         -          |         -          |         -          | :heavy_check_mark: |      10      |
|        52        |         -          | :heavy_check_mark: |         -          |         -          |         -          | :heavy_check_mark: |      10      |
|        53        | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          |         -          | :heavy_check_mark: |      12      |
|        54        |         -          | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          | :heavy_check_mark: |      11      |
|        55        | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          |         -          | :heavy_check_mark: |      13      |
|        56        |         -          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          | :heavy_check_mark: |      15      |
|        57        | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |         -          | :heavy_check_mark: |      17      |
|        58        |         -          | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |      19      |
|        59        | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |      21      |

These have been designed with the assumption that it is unlikely for humidity data to be useful without temperature, for air pressure to be useful without humidity and temperature, etc. If this is not the case, if more ports are designed, and/or if [new sensors are added](#new-port-or-sensor-schema-instructions) then try to fit them into this existing port schema or mimic it in a way that is logical and extendable.

### Sensor Data Payload Encoding

Data is MSB byte encoded into the payload buffer for transferring over LoRaWAN (see [sensorPortSchema](#sensorportschema) for how they're defined in code). As mentioned above, the port number indicates exactly what data is in the payload.

The encoding of each sensor - if included in the payload for that port number - is always in the order and of the form:

| Order<sup>$</sup> | Sensor Data                        | Total Byte(s) | Number of Values<sup>#</sup> | Scale Factor (Multiplier)  | Signed or Unsigned |
| :---------------: | ---------------------------------- | :-----------: | :--------------------------: | :------------------------: | :----------------: |
|         1         | Battery Voltage (mV)               |       2       |              1               |             1              |      Unsigned      |
|         2         | Temperature (°C)                   |       2       |              1               | 10<sup>2</sup><sup>^</sup> |       Signed       |
|         3         | Relative Humidity (%)              |       1       |              1               |     2.55<sup>\*</sup>      |      Unsigned      |
|         4         | Air Pressure (Pa)                  |       4       |              1               |             1              |      Unsigned      |
|         5         | Gas Resistance                     |       4       |              1               |             1              |      Unsigned      |
|         6         | Location (Latitude then Longitude) |       8       |              2               | 10<sup>4</sup><sup>^</sup> |       Signed       |

<sub><sup>$</sup> The order is listed here but in code is defined in the **port** encoding function - not the sensor.</sub>

<sub><sup>#</sup> The total bytes assigned to the sensor data are split equally amoungst the number of values it needs to represent</sub>

<sub><sup>^</sup> Only integer data can be encoded so the power is the number of decimal places sent with the data (10<sup>dp</sup>)- the rest of the precision is discarded</sub>

<sub><sup>\*</sup> Scale value to fill byte, e.g.: 0-100 -> 0-255</sub>

### Payload Data Format Examples

Some examples of what the payload for different ports looks like based on the port and sensor schema's defined above.

> e.g. PN = 5

|       Byte 0        |       Byte 1        |     Byte 2      |     Byte 3      |  Byte 4  |
| :-----------------: | :-----------------: | :-------------: | :-------------: | :------: |
| Battery Voltage MSB | Battery Voltage LSB | Temperature MSB | Temperature LSB | Humidity |

> e.g. PN = 8

|     Byte 0      |     Byte 1      |  Byte 2  |    Byte 3    |  Byte 4  |  Byte 5  |    Byte 6    |
| :-------------: | :-------------: | :------: | :----------: | :------: | :------: | :----------: |
| Temperature MSB | Temperature LSB | Humidity | Pressure MSB | Pressure | Pressure | Pressure LSB |

|     Byte 7      |   Byte 8    |   Byte 9    |     Byte 10     |
| :-------------: | :---------: | :---------: | :-------------: |
| Gas Resist. MSB | Gas Resist. | Gas Resist. | Gas Resist. LSB |

> e.g. PN = 53

|       Byte 0        |       Byte 1        |     Byte 2      |     Byte 3      |
| :-----------------: | :-----------------: | :-------------: | :-------------: |
| Battery Voltage MSB | Battery Voltage LSB | Temperature MSB | Temperature LSB |

|    Byte 4    |  Byte 5  |  Byte 6  |    Byte 7    |    Byte 8     |  Byte 9   |  Byte 10  |    Byte 11    |
| :----------: | :------: | :------: | :----------: | :-----------: | :-------: | :-------: | :-----------: |
| Latitude MSB | Latitude | Latitude | Latitude LSB | Longitude MSB | Longitude | Longitude | Longitude LSB |

#### Invalid Sensor Data

If the sensor data is not valid, for whatever reason, the bytes still need to be sent by the device to match the expected port payload format. To indicate that the value should be ignored by the decoder a value close to max will be encoded instead. Depending on whether the sensor data can be signed (as defined [above](#payload-encoding)) a segment of:

- **`0x7F7F7F7F`** if signed, or
- **`0xFFFFFFFF`** if unsigned

will be encoded that fills the number of bytes assigned to that sensor data and sent instead.

> E.g. For an invalid 2 byte signed the value will be 0x7f7f.

This has been elected as an alternative to changing the port number to match what sensor data is available, as otherwise it would be difficult to tell the difference between a sensor having issues and the wrong port being used. See the [suggested next steps for the decoder](https://github.com/minisolarunsw/LoRaWANProjectRepo/tree/main/Ubidots/PayloadDecoder/#suggested-next-steps) on ways the invalid data could be used more intelligently.

### portSchema

portSchema is a struct with the port number and series of flags that define which sensor data is included in the lora frame for that port number.

```c++
struct portSchema {
    uint8_t port_number;

    /**< Flags for if the sensors data is included in this port. */
    bool sendBatteryVoltage;
    bool sendTemperature;
    bool sendRelativeHumidity;
    bool sendAirPressure;
    bool sendGasResistance;
    bool sendLocation;

    /**
     * @brief Encodes the given sensor data into the payload according to the port's schema.
     * Calls sensorPortSchema::encodeData for each sensor.
     * @param sensor_data Sensor data to be encoded.
     * @param payload_buffer Payload buffer for data to be written into.
     * @param start_pos Start encoding data at this byte. Defaults to 0.
     * @return Total length of data encoded to payload_buffer.
     */
    uint8_t encodeSensorDataToPayload(sensorData *sensor_data, uint8_t *payload_buffer, uint8_t start_pos = 0);
};
```

To define a port, instantiate the portSchema struct with the port number and flags defined, e.g.:

```c++
const portSchema PORT1 = {
    1,     // port_number
    true,  // sendBatteryVoltage
    false, // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};
```

### sensorPortSchema

sensorPortSchema is a class with the port encoding settings for each sensor, plus the encoding function that uses those settings.

The overloads of encodeData() allow sensor data of various types to be encoded, and new overloads of the functions can be added if needed.

```c++
class sensorPortSchema {
  public:
    uint8_t n_bytes;    /**< Total length in payload - assumed to be split equally amongst n_values. */
    uint8_t n_values;   /**< Number of values sent for sensor data. */
    float scale_factor; /**< Only int values are encoded. To send a float value, mulitply by scale_factor to encode;
                             then divide by scale_factor to decode. */
    bool is_signed;     /**< Value has a sign and hence can be negative. */

    ...
    /**
     * @brief Byte encodes the given sensor data into the payload according to the sensor port schema.
     * Calls a template function defined in PortSchema.cpp that can take in sensor_data of various types.
     * ...
     * @param sensor_data Sensor data to encode (valid data types: int, float, uint8_t, uint16_t, uint32_t).
     * @param valid Validity of given sensor data.
     * @param payload_buffer Payload buffer for data to be written into.
     * @param current_buffer_len Length of current data in the buffer, used to avoid overwriting data.
     * @return Total length of data encoded to payload_buffer.
     */
    uint8_t encodeData(<type> sensor_data, bool valid, uint8_t *payload_buffer) const;

};
```

The sensorPortSchema of each sensor is defined once as an instance of the sensorPortSchema class. These definitions are summarised in the [table above](#payload-encoding). E.g.:

```c++
static const sensorPortSchema temperatureSchema = { // units: degrees C
    .n_bytes = 2,
    .n_values = 1,
    .scale_factor = pow(10.0, 2), // 2 decimal places
    .is_signed = true
};
```

If one needs to be modified (e.g. the number of bytes, scaling factor, etc.) or a [new sensor added](#new-port-or-sensor-schema-instructions) this needs to be done in the SensorPortSchema.h file.

### New Port or Sensor Schema Instructions

To add a new sensor, it is best practice to define a new port that includes the new sensor with whatever combination of other sensors is desired - instead of redefining an existing port. Once you have decided on the new port, assign it a new port number (following the rules above), then to define it in the firmware:

1. Add a new sensorPortSchema: `static const sensorPortSchema newSensorSchema = {...};` and add the sensor to the `sensorData` struct, copying the same format:

   ```c++
   ...
   struct {
       float value;
       bool is_valid;
   } new_sensor;
   ...
   ```

2. Add the corresponding enabled flag to the portSchema class.
3. Set the enabled flag for the new sensor to false in all existing defined ports. TIP: Replace (ctrl + h) or multi-cursor editting (alt + left click) are very handy for this.
4. Add the new port with `const portSchema PORTX = {...};`, replacing `X` with the new port number.
5. Finally add the port to the [decoder on the web-app side](https://github.com/minisolarunsw/LoRaWANProjectRepo/tree/main/Ubidots/PayloadDecoder).

You should also update the table(s) above with the new port/sensor schema.

## Suggested Next Steps

Obivously the port & sensor schema's have been set up around sensor data, however with a bit of creativity they can also easily be adapted to send other data that makes sense to be sent regularly/accompany sensor data e.g. if an RTC was added to the devices then a timestamp could be added to the schema, assigned a port number and included in the payload; or likewise if a _short_ string (not really something lora is good at), or ping of some sort needs to be sent it, too could be added to the schema, assigned a port number and included in the payload. Either way the port number that is sent with every payload should be used as a tool and not just a wasted byte of data.
