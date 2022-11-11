#include "RAK1906_helper.h"

bool RAK1906::init(initRAK1906Sensors *initSensors) {
    Wire.begin();
    if (!begin(BME680_ADDRESS)) {
        log(LOG_LEVEL::ERROR, "Could not find a valid BME680 sensor, check wiring!");
        return false;
    }

    // Set up oversampling and filter initialization
    if (initSensors->temp) {
        setTemperatureOversampling(TEMP_OVERSAMPLING);
    } else {
        // disable sensor
        setTemperatureOversampling(BME68X_OS_NONE);
    }
    if (initSensors->humi) {
        setHumidityOversampling(HUMI_OVERSAMPLING);
    } else {
        // disable sensor
        setHumidityOversampling(BME68X_OS_NONE);
    }
    if (initSensors->pres) {
        setPressureOversampling(PRES_OVERSAMPLING);
    } else {
        // disable sensor
        setPressureOversampling(BME68X_OS_NONE);
    }
    if (initSensors->gas) {
        setIIRFilterSize(GAS_IIR_FILTER);
        setGasHeater(GAS_HEATER_TEMP_C, GAS_HEATER_TIME_MS);
    } else {
        // disable sensor
        setGasHeater(0, 0);
    }
    return true;
}
