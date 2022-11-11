/**
 * @file SensorHelper.h
 * @author Kalina Knight
 * @brief Functions and varaibles related to reading the sensors.
 *
 * NOTE: References to the GPS & GPSClass are a sudocode example of a new sensor and should be replaced with actual code
 * to operate the GPS (or another new sensor). The GPSClass could be created in a similar fashion to the RAK1901 &
 * RAK1906 classes that inherit the available Arduino sensor libraries and add additional functions to simplify
 * initialisation and reading.
 *
 * WARNING: Sensors that are plugged in, but not in use can waste a fair amount of power. Some WisBlock sensors do not
 * default to their low power/idle state on power up. If they are not in use by the port then they will not be
 * initialised and put into their idle state manually by the firmware, and hence will waste a lot of power doing
 * nothing.
 *
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include "AnalogSensor.h"   /**< Class to read a sensor using the onboard ADC. Plus BatteryLevel class. */
#include "Logging.h"        /**< Go here to change the logging level for the entire application. */
#include "PortSchema.h"     /**< Go here for portSchema definitions. */
#include "RAK1901_helper.h" /**< Wrapper for SHTC3 library. */
#include "RAK1906_helper.h" /**< Wrapper for BME680 library. */

/**
 * @brief Initialise the given sensors based on the port schema.
 * As there are two sensors (1901 & 1906) that can provide temp & humi data, a sensor must be specified.
 * @param port_settings Pointer to port schema for this app.
 * @param useRAK1901
 * @param useRAK1906
 * @return True if successful. False if not.
 */
bool initSensors(const portSchema *port_settings, bool useRAK1901, bool useRAK1906);

/**
 * @brief Get the sensor data.
 * @param port_settings Pointer to port schema for this app.
 * @return The sensor data in sensorData struct format.
 */
sensorData getSensorData(const portSchema *port_settings);
