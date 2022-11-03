#ifndef PORT_SCHEMA_H
#define PORT_SCHEMA_H

/**
 * @file PortSchema.h
 * @author Kalina Knight (kalina.knight77@gmail.com)
 * @brief Port schema definition as descibed the README.
 * Schema's include the functions for encoding the data to the LoRaWAN payload as well.
 *
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino

#include "Logging.h"          /**< Go here to change the logging level for the entire application. */
#include "SensorPortSchema.h" /**< Go here for the individual sensor schema definitions. */

/** @brief portSchema describes which sensor data to include in each port and hence the payload. */
class portSchema {
  public:
    uint8_t port_number;

    /**< Flags for if the sensors data is included in this port. */
    bool sendBatteryVoltage;
    bool sendTemperature;
    bool sendRelativeHumidity;
    bool sendAirPressure;
    bool sendGasResistance;
    bool sendLocation;
    /* An example of a new sensor:
    bool sendNewSensor;
    */

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// SCHEMA DEFINITIONS: See readme for definitions in tabular format.

const portSchema PORT1 = {
    1,     // port_number
    true,  // sendBatteryVoltage
    false, // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};

const portSchema PORT2 = {
    2,     // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};

const portSchema PORT3 = {
    3,     // port_number
    true,  // sendBatteryVoltage
    true,  // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};

const portSchema PORT4 = {
    4,     // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};

const portSchema PORT5 = {
    5,     // port_number
    true,  // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};

const portSchema PORT6 = {
    6,     // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    true,  // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};
const portSchema PORT7 = {
    7,     // port_number
    true,  // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    true,  // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};

const portSchema PORT8 = {
    8,     // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    true,  // sendAirPressure
    true,  // sendGasResistance
    false  // sendLocation
};

const portSchema PORT9 = {
    9,    // port_number
    true, // sendBatteryVoltage
    true, // sendTemperature
    true, // sendRelativeHumidity
    true, // sendAirPressure
    true, // sendGasResistance
    false // sendLocation
};

const portSchema PORT50 = {
    50,    // port_number
    false, // sendBatteryVoltage
    false, // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};

const portSchema PORT51 = {
    51,    // port_number
    true,  // sendBatteryVoltage
    false, // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};

const portSchema PORT52 = {
    52,    // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};

const portSchema PORT53 = {
    53,    // port_number
    true,  // sendBatteryVoltage
    true,  // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};

const portSchema PORT54 = {
    54,    // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};

const portSchema PORT55 = {
    55,    // port_number
    true,  // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};

const portSchema PORT56 = {
    56,    // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    true,  // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};
const portSchema PORT57 = {
    57,    // port_number
    true,  // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    true,  // sendAirPressure
    false, // sendGasResistance
    true   // sendLocation
};

const portSchema PORT58 = {
    58,    // port_number
    false, // sendBatteryVoltage
    true,  // sendTemperature
    true,  // sendRelativeHumidity
    true,  // sendAirPressure
    true,  // sendGasResistance
    true   // sendLocation
};

const portSchema PORT59 = {
    59,   // port_number
    true, // sendBatteryVoltage
    true, // sendTemperature
    true, // sendRelativeHumidity
    true, // sendAirPressure
    true, // sendGasResistance
    true  // sendLocation
};

/* An example of a new port:
const portSchema PORTX = {
    X,     // port_number
    false, // sendBatteryVoltage
    false, // sendTemperature
    false, // sendRelativeHumidity
    false, // sendAirPressure
    false, // sendGasResistance
    false  // sendLocation
};
*/

#endif // PORT_SCHEMA_H