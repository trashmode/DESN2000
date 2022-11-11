/**
 * @file RAK1906_helper.h
 * @author Kalina Knight
 * @brief RAK1906 class inherits the Adafruit_BME680 class and adds functions to simplify initialisation and reading for
 * the SensorHelper application.
 *
 * WARNING: A warning about the gas resistance sensor. To measure the gas resistance a small heater is used, not only
 * does this use a lot of power, but it also can ruin the readings from the other sensors (seeing as the local
 * environment near the heater will not be the same as the general environment). The value also cannot be correlated to
 * any real value in this implementation (see this forum post:
 * https://forum.rakwireless.com/t/rak7204-gas-resistance-what-should-the-values-tell-me/1716).
 * Hence the gas resistance can only be used in the context of other readings to show a relative change.
 *
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <Adafruit_BME680.h>

#include "Logging.h"

typedef struct initRAK1906Sensors {
    bool temp;
    bool humi;
    bool pres;
    bool gas;
} initRAK1906Sensors;

// RAK1906 - environment sensor (temperature, humidity, pressure, gas resistance)
class RAK1906 : public Adafruit_BME680 {
  private:
    // Feel free to change these settings - just refer to the manual and Adafruit_BME680 library
    const uint8_t BME680_ADDRESS = 0x76;
    const uint8_t TEMP_OVERSAMPLING = BME680_OS_8X;
    const uint8_t HUMI_OVERSAMPLING = BME680_OS_2X;
    const uint8_t PRES_OVERSAMPLING = BME680_OS_4X;
    const uint8_t GAS_IIR_FILTER = BME680_FILTER_SIZE_3;
    const uint16_t GAS_HEATER_TEMP_C = 320;  // gas heater temperature
    const uint16_t GAS_HEATER_TIME_MS = 150; // gas heater time on

  public:
    /**
     * @brief Initialise the environmental sensing unit.
     * Sets the oversampling of the temperature, humidity & pressure sensors.
     * Plus sets the IIR filter size & heater settings for the gas resistance sensor.
     * Calls Wire.begin() in the process.
     * @param initSensors Struct of flags indicating which sensors should be enabled.
     * @return True if successful. False if not.
     */
    bool init(initRAK1906Sensors *initSensors);

    /**
     * @brief Gets the environmental sensing unit data ready.
     * @return True if data is ready. False if not.
     */
    inline bool dataReady(void) { return performReading(); };

    /**
     * @brief Get temperature.
     * @return Temperature in degrees celcius.
     */
    inline float getTemperature(void) { return temperature; };

    /**
     * @brief Get humidity.
     * @return Relative humidity as a percentage.
     */
    inline float getHumidity(void) { return humidity; };

    /**
     * @brief Get air pressure.
     * @return Air pressure in Pa.
     */
    inline uint32_t getPressure(void) { return pressure; };

    /**
     * @brief Get gas resistance.
     * @return Technically returns gas resistance in ohms, but this cannot be correlated to anything meaningful.
     * See top of file.
     */
    inline uint32_t getGasResistance(void) { return gas_resistance; };
};