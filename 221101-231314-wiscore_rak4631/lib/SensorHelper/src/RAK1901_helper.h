/**
 * @file RAK1901_helper.h
 * @author Kalina Knight
 * @brief RAK1901 class inherits the SHTC3 class and adds functions to simplify initialisation and reading for the
 * SensorHelper application.
 *
 * @version 0.1
 * @date 2021-08-13
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <SparkFun_SHTC3.h>

#include "Logging.h"

class RAK1901 : public SHTC3 {
  public:
    /**
     * @brief Initialises the temperature & humidity sensor.
     * Calls Wire.begin() in the process.
     * @return True if successfull. False if not.
     */
    bool init(void);

    /**
     * @brief Gets the temperature & humidity data ready.
     * @return True if data is ready. False if not.
     */
    bool dataReady(void);

    /**
     * @brief Get temperature.
     * @return Temperature in degrees celcius.
     */
    inline float getTemperature(void) { return toDegC(); };

    /**
     * @brief Get humidity.
     * @return Relative humidity as a percentage.
     */
    inline float getHumidity(void) { return toPercent(); };
};
