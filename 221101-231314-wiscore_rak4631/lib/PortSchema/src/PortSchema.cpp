#include "PortSchema.h"
// #include <stdint.h>

uint8_t portSchema::encodeSensorDataToPayload(sensorData *sensor_data, uint8_t *payload_buffer, uint8_t start_pos) {
    /* If the data should be included in the payload according to the portSchema, then encode the value.
     * he order of these if statements is the order the sensor data will be encoded into the payload and should match
     * the schema.
     * The buffsize is increased by the amount of data encoded in each step.
     */
    uint8_t payload_length = start_pos;
    if (sendBatteryVoltage) {
        payload_length = batteryVoltageSchema.encodeData(
            sensor_data->battery_mv.value, sensor_data->battery_mv.is_valid, payload_buffer, payload_length);
    }
    if (sendTemperature) {
        payload_length = temperatureSchema.encodeData(sensor_data->temperature.value, sensor_data->temperature.is_valid,
                                                      payload_buffer, payload_length);
    }
    if (sendRelativeHumidity) {
        payload_length = relativeHumiditySchema.encodeData(sensor_data->humidity.value, sensor_data->humidity.is_valid,
                                                           payload_buffer, payload_length);
    }
    if (sendAirPressure) {
        payload_length = airPressureSchema.encodeData(sensor_data->pressure.value, sensor_data->pressure.is_valid,
                                                      payload_buffer, payload_length);
    }
    if (sendGasResistance) {
        payload_length = gasResistanceSchema.encodeData(sensor_data->gas_resist.value, sensor_data->gas_resist.is_valid,
                                                        payload_buffer, payload_length);
    }
    if (sendLocation) {
        payload_length = locationSchema.encodeData(sensor_data->location.latitude, sensor_data->location.is_valid,
                                                   payload_buffer, payload_length);
        payload_length = locationSchema.encodeData(sensor_data->location.longitude, sensor_data->location.is_valid,
                                                   payload_buffer, payload_length);
    }
    if (sendTurbidity) {
        payload_length = turbiditySchema.encodeData(sensor_data->turbidity.value, sensor_data->turbidity.is_valid,
                                                   payload_buffer, payload_length);
    }
    return payload_length;
};