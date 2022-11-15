#include "SensorHelper.h"

/**
 * @brief Temp & Humi Sensor Selection
 * As there are two different sensors that can provide temperature &/or humidity the user must specify which one to use.
 * If both USERAK1901 & USERAK1906 are true then we default to using the RAK1906 as it has the larger sensor array.
 */
bool USERAK1901 = false;
bool USERAK1906 = false;

/**
 * @brief Sensor object instantiations.
 * NOTE: Instantiation does not equal initialisation of the sensor. The instantiation does not interact with the sensor
 * itself, instead it just creates the memory container for interating with it.
 */
RAK1901 tempHumiSensor;
RAK1906 enviroSensor;
BatteryLevel batLvl;
TurbidityLevel turbLvl;
// GPSClass gps;
// AnalogSensor analogsensorexample(sensor A1, ADC reference voltage, ADC 10, ADC oversampling);

bool initSensors(const portSchema *port_settings, bool useRAK1901, bool useRAK1906) {
    log(LOG_LEVEL::DEBUG, "Initialising sensors...");

    if (useRAK1901 && useRAK1906) {
        log(LOG_LEVEL::WARN, "Cannot use both SHTC3(RAK1901) & BME680(RAK1906). The RAK1906 will be used by default.");
        USERAK1906 = useRAK1906;
        USERAK1901 = false;
    } else {
        USERAK1901 = useRAK1901;
        USERAK1906 = useRAK1906;
    }

    // battery voltage setup
    if (port_settings->sendBatteryVoltage) {
        batLvl.ADCInit();
    }

    // 1906 or 1901 setup
    if (port_settings->sendTemperature || port_settings->sendRelativeHumidity || port_settings->sendAirPressure ||
        port_settings->sendGasResistance) {
        if (USERAK1906) {
            // Environmental (RAK1906) sensor setup
            initRAK1906Sensors init_sensors = {
                port_settings->sendTemperature,
                port_settings->sendRelativeHumidity,
                port_settings->sendAirPressure,
                port_settings->sendGasResistance,
            };
            if (!enviroSensor.init(&init_sensors)) {
                log(LOG_LEVEL::ERROR, "Unable to initialise the RAK1906.");
                return false;
            }
        } else if (USERAK1901) {
            if (port_settings->sendAirPressure || port_settings->sendGasResistance) {
                log(LOG_LEVEL::ERROR, "The RAK1901 sensor cannot provide air pressure or gas resistance.");
                return false;
            } else if (port_settings->sendTemperature || port_settings->sendRelativeHumidity) {
                // Temperature and humidity (tempHumiSensor) sensor setup
                if (!tempHumiSensor.init()) {
                    log(LOG_LEVEL::ERROR, "Unable to initialise the RAK1901.");
                    return false;
                }
            }
        } else {
            log(LOG_LEVEL::ERROR, "No sensor chosen to read temp/humi/pressure/gas.");
            return false;
        }
    } else if (USERAK1901 || USERAK1906) {
        log(LOG_LEVEL::WARN, "Neither a RAK1901 or RAK1906 is required for this port.");
    }

    // if (port_settings->sendLocation) {
    //     gps.init();
    // }
    if (port_settings->sendTurbidity){
        turbLvl.ADCInit();
    }

    return true;
}

sensorData getSensorData(const portSchema *port_settings) {
    sensorData data = {};

    if (port_settings->sendBatteryVoltage) {
        data.battery_mv.value = batLvl.getSensorMV();
        data.battery_mv.is_valid = true;
    }

    if (port_settings->sendTemperature || port_settings->sendRelativeHumidity || port_settings->sendAirPressure ||
        port_settings->sendGasResistance) {
        if (USERAK1906) {
            if (enviroSensor.dataReady()) {
                if (port_settings->sendTemperature) {
                    data.temperature.value = enviroSensor.getTemperature();
                    data.temperature.is_valid = true;
                }
                if (port_settings->sendRelativeHumidity) {
                    data.humidity.value = enviroSensor.getHumidity();
                    data.humidity.is_valid = true;
                }
                if (port_settings->sendAirPressure) {
                    data.pressure.value = enviroSensor.getPressure();
                    data.pressure.is_valid = true;
                }
                if (port_settings->sendGasResistance) {
                    data.gas_resist.value = enviroSensor.getGasResistance();
                    data.gas_resist.is_valid = true;
                }
            }
        } else if (USERAK1901) {
            if (tempHumiSensor.dataReady()) {
                if (port_settings->sendTemperature) {
                    data.temperature.value = tempHumiSensor.getTemperature();
                    data.temperature.is_valid = true;
                }
                if (port_settings->sendRelativeHumidity) {
                    data.humidity.value = tempHumiSensor.getHumidity();
                    data.humidity.is_valid = true;
                }
            }
        }
    }

    // if (port_settings->sendLocation) {
    //     if (valid gps data) {
    //         data.location.latitude = gps.getLatitude();
    //         data.location.longitude = gps.getLongitude();
    //         data.location.is_valid = true;
    //     }
    // }
    if (port_settings->sendTurbidity) {             // Takes 100 measurments and sends the avg turbidity
        float sum = 0;
        for (int i = 0; 100 > i; i++) {
            float sensor = turbLvl.getSensorMV();
            sum = sum + turbLvl.mvToNTU(sensor);
            log(LOG_LEVEL::DEBUG,"%d",i);
            delay(100);
        }
        data.turbidity.is_valid = true;
        data.turbidity.value = sum/100;
    }
    return data;
}