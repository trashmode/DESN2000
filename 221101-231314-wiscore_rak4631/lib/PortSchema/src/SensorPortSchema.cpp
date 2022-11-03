#include "SensorPortSchema.h"

/**
 * @brief Byte encodes the given sensor data into the payload according to the given sensor port schema.
 * If the sensor data is not valid, for whatever reason, a value close to max (for the number of bytes) will be
 * encoded instead. The decoder then knows to ignore the data as it is invalid. If the data is invalid a segment of
 * 0x7F7F7F7F (signed) or 0xFFFFFFFF (unsigned) will be encoded and sent instead of the invalid data. E.g. For an
 * invalid 2 byte signed the value will be 0x7f7f.
 * @param sensor_data Sensor data to encode. This template allows the type of sensor_data to be flexible (to a point).
 * @param valid Validity of given sensor data.
 * @param payload_buffer LoRaWAN payload with buffer for data to be written into.
 * @param sensor_schema Sensor port schema that determines how the data is encoded.
 * @return New total length of data encoded to payload_buffer.
 */
template <typename T>
uint8_t encodeDataWithSchema(T sensor_data, bool valid, uint8_t *payload_buffer, uint8_t buf_pos, const sensorPortSchema *sensor_schema) {
    int data_to_encode = 0;
    // Check validity
    if (valid) {
        /* Perform float maths to scale the data and assign the result to an int.
         * This discards any decimal values not captured by the scale factor */
        data_to_encode = (int)(((float)sensor_data) * sensor_schema->scale_factor);
    } else {
        /* If the data is invalid, a (close to) max value will be sent through.
         * A max value received by the decoder should be ignored */
        if (sensor_schema->is_signed) {
            // 0x7f... must be sent instead of 0xff for signed values as the first bit is used to indicate sign
            data_to_encode = 0x7F7F7F7F;
        } else {
            data_to_encode = 0xFFFFFFFF;
        }
    }

    if (!sensor_schema->is_signed && (data_to_encode < 0)) {
        log(LOG_LEVEL::WARN, "A signed value is being sent for a sensor port schema that is unsigned.");
    }

    // The total bytes assigned to the sensor is assumed to be split equally amongst the number of values used
    // to represent the sensor data.
    int data_size = sensor_schema->n_bytes / sensor_schema->n_values;

    // Bitwise encode the data
    uint8_t i = 0;
    uint8_t j = (data_size - 1);
    for (; i < data_size; i++, j--) {
        // This is a generalised form of basic MSB bitwise encoding
        uint8_t bitshift = j * 8; // 8 bits in a byte
        if (j > 0) {
            payload_buffer[buf_pos + i] = (uint8_t)((data_to_encode & (0xFF << bitshift)) >> bitshift);
        } else {
            payload_buffer[buf_pos + i] = (uint8_t)(data_to_encode & 0xFF);
        }
    }

    // return the new buffer length
    return (buf_pos + i);
}

/**
 * NOTE:'this' will refer to the sensorPortSchema instance that is making the call to encodeData.
 * e.g. In the call batteryVoltageSchema.encodeData(...), 'this' refers to the batteryVoltageSchema instance of
 * sensorPortSchema.
 */

uint8_t sensorPortSchema::encodeData(uint8_t sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const {
    return (encodeDataWithSchema(sensor_data, valid, payload_buffer, current_buffer_len, this));
}

uint8_t sensorPortSchema::encodeData(uint16_t sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const {
    return (encodeDataWithSchema(sensor_data, valid, payload_buffer, current_buffer_len, this));
}

uint8_t sensorPortSchema::encodeData(uint32_t sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const {
    return (encodeDataWithSchema(sensor_data, valid, payload_buffer, current_buffer_len, this));
}

uint8_t sensorPortSchema::encodeData(int sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const {
    return (encodeDataWithSchema(sensor_data, valid, payload_buffer, current_buffer_len, this));
}

uint8_t sensorPortSchema::encodeData(float sensor_data, bool valid, uint8_t *payload_buffer, uint8_t current_buffer_len) const {
    return (encodeDataWithSchema(sensor_data, valid, payload_buffer, current_buffer_len, this));
}
