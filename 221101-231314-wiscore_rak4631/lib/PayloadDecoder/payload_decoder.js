/**
 * Payload decoder for Ubidots. See README for details.
 * If switching to a different platform decodePayload(), sensorPortSchema's & portSchema's can be
 * copied and modified to suit new platforms expecting format.
 */

// For debugging purposes
const COLLECT_GATEWAY_METADATA = true; // set to false to save data points
const DEBUGGING = false; // set to true to see console debugging
function debugLog(string) {
  if (DEBUGGING) {
    console.debug(string);
  }
}

/**
 * Function format_payload():
 * Equivalent to main() for this code.
 * Grabs the TTS payload and reformats it into a Ubidots-compatible JSON.
 * @param {*} args
 * @returns The Ubidots formatted JSON.
 */
function format_payload(args) {
  let ubidots_payload = {};

  // Get uplink's timestamp
  ubidots_payload["timestamp"] = new Date(
    args["uplink_message"]["received_at"]
  ).getTime();

  // get bytes to decode
  let bytes = Buffer.from(args["uplink_message"]["frm_payload"], "base64");
  let f_port = args["uplink_message"]["f_port"];
  // decode byte payload
  let decoded_payload = decodePayload(bytes, f_port);

  // Use this instead if you're already decoding in TTS using payload formatters.
  // PROTIP: Make sure the incoming decoded payload is an Ubidots-compatible JSON (See https://ubidots.com/docs/hw/#sending-data)
  // let decoded_payload = args['uplink_message']['decoded_payload'];

  // add gateway data
  if (COLLECT_GATEWAY_METADATA) {
    let gateways = args["uplink_message"]["rx_metadata"];
    let [gateway_value, gateway_context] = getGatewayMetadata(gateways);
    ubidots_payload["gateways"] = {
      value: gateway_value,
      context: gateway_context,
    };
  }

  // Merge decoded payload into Ubidots payload
  Object.assign(ubidots_payload, decoded_payload);
  debugLog("Payload for ubidots: " + ubidots_payload);
  return ubidots_payload;
}

const BITS_IN_BYTE = 8;

/**
 * Template class sensorPortSchema:
 * Used to define the schema for each sensor below this class.
 */
class sensorPortSchema {
  /**
   * Constructor for sensorPortSchema:
   * @param {*} n_bytes        Total data length in payload - assumed to be split equally amongst n_values.
   * @param {*} n_values       Number of values sent for sensor data.
   * @param {*} scale_factor   Only int values are encoded. For float values, divide by scale_factor to decode.
   * @param {*} signed         Value has a sign and hence can be negative.
   * @param {*} decimal_places Default to null. Specify number of decimal places to truncate value. Fixes noise that results from decoding.
   */
  constructor(n_bytes, n_values, scale_factor, signed, decimal_places = null) {
    // check if inputs are valid
    if (n_bytes < 1) {
      debugLog("Error: n_bytes min is 1. n_bytes set to 1.");
      n_bytes = 1;
    }
    if (n_values < 1) {
      debugLog("Error: n_values min is 1. n_values set to 1.");
      n_values = 1;
    }
    if (scale_factor == 0) {
      debugLog("Error: scale_factor cannot be 0. scale_factor set to 1.");
      scale_factor = 1;
    }
    if (decimal_places < 0) {
      debugLog(
        "Error: decimal_places cannot be less than 0. decimal_places set to " +
          Math.abs(decimal_places) +
          "."
      );
      decimal_places = Math.abs(decimal_places);
    }
    // assign values
    this.n_bytes = n_bytes;
    this.n_values = n_values;
    this.scale_factor = scale_factor;
    this.signed = !!signed; // convert to boolean
    this.decimal_places = decimal_places;
    // n_bytes is assumed to be split equally amongst the values
    this.n_bytes_per_value = this.n_bytes / this.n_values;
  }

  /**
   * Method decodeValue():
   * Decodes the given bytes into the sensor value(s) using decodeBytes() and applies
   * the scaling_factor to the result(s).
   * @param {*} bytes      Byte buffer of data.
   * @param {*} start_byte Value(s) start position in buffer.
   * @returns The value if n_values == 1. Otherwise returns a values array with n_values elements.
   */
  decodeValue(bytes, start_byte) {
    let values = [null];
    for (let i = 0; i < this.n_values; i++) {
      // decode 1 value from buffer
      values[i] = this.decodeBytes(
        bytes,
        start_byte + i * this.n_bytes_per_value // move forward by n_bytes_per_value each iteration of loop
      );
      if (this.validData(values[i])) {
        // divide by scaling_factor to undo the scaling done by the device
        values[i] /= this.scale_factor;
        // then use the decimal_places (if present) to correct for decoding noise
        if (this.decimal_places !== null) {
          // toFixed() returns a string
          values[i] = Number(values[i].toFixed(this.decimal_places));
        }
      } else {
        values[i] = null;
      }
    }

    // if only one value then just return the value, not the list
    if (this.n_values == 1) {
      return values[0];
    }
    // return the whole list of values
    return values;
  }

  /**
   * Method decodeBytes():
   * Takes in the bytes and bitshifts them back together to give the value.
   * Can process signed or unsigned values (defaults to unsigned).
   *
   * This function is the signed, generalised equivalent of basic bitwise assignment:
   * let Uint32 = // uint32_t example
   *   (bytes[start_byte + 0] << 24) |
   *   (bytes[start_byte + 1] << 16) |
   *   (bytes[start_byte + 2] << 8) |
   *   bytes[start_byte + 3];
   *
   * @param {*} bytes      Byte buffer of data.
   * @param {*} start_byte Value start position in buffer.
   * @param {*} v_bytes    Number of bytes used for value.
   * @returns The value.
   */
  decodeBytes(bytes, start_byte, v_bytes = this.n_bytes_per_value) {
    let value = 0;
    let is_negative = false;

    // if data is signed then the first bit indicates it's sign
    if (this.signed && bytes[start_byte] & 0x80) {
      is_negative = true;
      // Negative numbers are stored as the two's complement,
      // hence start with the two's complement
      value = ~value;
    }

    // bit shift each byte of LSB value and add to end result
    for (let b = 0, s = v_bytes - 1; b < v_bytes; b++, s--) {
      if (is_negative) {
        // clear the corresponding byte in value so that the | (bitwise OR)
        // copies over the target byte to value
        value &= ~(0xff << (s * BITS_IN_BYTE));
      }
      // bitshift to correct position and bitwise OR with value to copy over.
      value |= bytes[start_byte + b] << (s * BITS_IN_BYTE);
    }

    return value;
  }

  /**
   * Method validData():
   * Checks if the given value is valid.
   * The end node will send through a value close to the max (for the number of bytes) to indicate
   * that the data is not valid. This will be a segment of 0x7f7f7f7f (signed) or 0xffffffff (unsigned).
   * E.g. For an invalid 2 byte signed value the node will send 0x7f7f.
   *
   * NOTE: If the value has been encoded in only one byte the function always returns true.
   * @param {*} value   Value to validate.
   * @param {*} v_bytes Number of bytes used for value.
   * @returns True if valid, false if not.
   */
  validData(value, v_bytes = this.n_bytes_per_value) {
    // it's difficult to indicate a sensors data is invalid if encoded in 1 byte as it's likely the entire range is needed
    if (v_bytes == 1) {
      return true;
    }
    let data_invalidator = 0;
    for (let n = 0; n < v_bytes; n++) {
      if (this.signed) {
        data_invalidator |= 0x7f << (n * BITS_IN_BYTE);
      } else {
        data_invalidator |= 0xff << (n * BITS_IN_BYTE);
      }
    }
    return !!(value - data_invalidator);
  }
}

/**
 * Sensor Port Schema definitions for each sensor.
 * Mirrors what's in the device firmware from SensorHelper lib.
 */
const batteryVoltageSchema = new sensorPortSchema(2, 1, 1, false);
const temperatureSchema = new sensorPortSchema(2, 1, 10 ** 2, true);
const relativeHumiditySchema = new sensorPortSchema(1, 1, 2.55, false, 2); // Only humidity needs the decimal places to correct the noise that results from decoding.
const airPressureSchema = new sensorPortSchema(4, 1, 1, false);
const gasResistanceSchema = new sensorPortSchema(4, 1, 1, false);
const locationSchema = new sensorPortSchema(8, 2, 10 ** 4, true);
// const newSensorSchema = new sensorPortSchema(1, 1, 1, false);

/**
 * Template class portSchema:
 * Used to define the schema for each port below this class in PORT_SCHEMA.
 */
class portSchema {
  /**
   * Constructor for PortSchema:
   * Creates object variables and defaults them be false if not specified in options.
   * @param {*} options Object instantiation of {batteryVoltage, temperature, relativeHumidity,
   *                    airPressure, gasResistance, location}
   */
  constructor(options = {}) {
    Object.assign(
      this,
      {
        batteryVoltage: false,
        temperature: false,
        relativeHumidity: false,
        airPressure: false,
        gasResistance: false,
        location: false,
      },
      options
    );
  }
}

/**
 * Each PORT in PORT_SCHEMA is a series of flags indicating which sensor data is included in the port.
 * They all default to false unless set to true.
 * Mirrors what's in the device firmware from SensorHelper lib.
 */
// prettier-ignore
// ^ I use Prettier to format my .js files, but I don't want it to format PORT_SCHEMA
const PORT_SCHEMA = {
  PORT1:  new portSchema({batteryVoltage: true                                                                                                   }),
  PORT2:  new portSchema({                    temperature: true                                                                                }),
  PORT3:  new portSchema({batteryVoltage: true, temperature: true                                                                                }),
  PORT4:  new portSchema({                    temperature: true, relativeHumidity: true                                                        }),
  PORT5:  new portSchema({batteryVoltage: true, temperature: true, relativeHumidity: true                                                        }),
  PORT6:  new portSchema({                    temperature: true, relativeHumidity: true, airPressure: true                                     }),
  PORT7:  new portSchema({batteryVoltage: true, temperature: true, relativeHumidity: true, airPressure: true                                     }),
  PORT8:  new portSchema({                    temperature: true, relativeHumidity: true, airPressure: true, gasResistance: true                }),
  PORT9:  new portSchema({batteryVoltage: true, temperature: true, relativeHumidity: true, airPressure: true, gasResistance: true                }),
  PORT50: new portSchema({                                                                                                       location: true}),
  PORT51: new portSchema({batteryVoltage: true,                                                                                    location: true}),
  PORT52: new portSchema({                    temperature: true,                                                                 location: true}),
  PORT53: new portSchema({batteryVoltage: true, temperature: true,                                                                 location: true}),
  PORT54: new portSchema({                    temperature: true, relativeHumidity: true,                                         location: true}),
  PORT55: new portSchema({batteryVoltage: true, temperature: true, relativeHumidity: true,                                         location: true}),
  PORT56: new portSchema({                    temperature: true, relativeHumidity: true, airPressure: true,                      location: true}),
  PORT57: new portSchema({batteryVoltage: true, temperature: true, relativeHumidity: true, airPressure: true,                      location: true}),
  PORT58: new portSchema({                    temperature: true, relativeHumidity: true, airPressure: true, gasResistance: true, location: true}),
  PORT59: new portSchema({batteryVoltage: true, temperature: true, relativeHumidity: true, airPressure: true, gasResistance: true, location: true})
  // PORTX:  new portSchema({ indicate which sensor data is included })
}

/**
 * Function decodePayload()
 * Decodes the given data payload depending on the port number.
 * Refer to payload_formatting.xlsx for formatting of each port.
 * @param {*} bytes Byte data payload.
 * @param {*} port_num  Port number of data. This is use to distinguish payload formatting.
 * @returns Decoded payload.
 */
function decodePayload(bytes, port_num) {
  let decoded = {}; // final decoded object
  let b = 0; // byte iterator

  // which port has the data come from
  let port_name = "PORT" + port_num; // i.e. if port_num = 1 then port_name = "PORT1"
  // test that the port is defined in PORT_SCHEMA
  if (!PORT_SCHEMA.hasOwnProperty(port_name)) {
    debugLog("Error: Port number not defined in PORT_SCHEMA.");
    return;
  }
  let port_format = PORT_SCHEMA[port_name];

  // check if the port includes that sensor
  // then decodeValue and move forward in bytes by size of the sensor value
  if (port_format.batteryVoltage) {
    decoded.battery_mv = batteryVoltageSchema.decodeValue(bytes, b);
    b += batteryVoltageSchema.n_bytes;
  }
  if (port_format.temperature) {
    decoded.temperature = temperatureSchema.decodeValue(bytes, b);
    b += temperatureSchema.n_bytes;
  }
  if (port_format.relativeHumidity) {
    decoded.humidity = relativeHumiditySchema.decodeValue(bytes, b);
    b += relativeHumiditySchema.n_bytes;
  }
  if (port_format.airPressure) {
    decoded.pressure = airPressureSchema.decodeValue(bytes, b);
    b += airPressureSchema.n_bytes;
  }
  if (port_format.gasResistance) {
    decoded.gas = gasResistanceSchema.decodeValue(bytes, b);
    b += gasResistanceSchema.n_bytes;
  }
  if (port_format.location) {
    // as location has two values (n_values = 2) an array is returned by decodeValues
    // these are assigned to latitude & longtiude respectively
    let [latitude, longitude] = locationSchema.decodeValue(bytes, b);
    b += locationSchema.n_bytes;

    // combined together into context data for an empty variable called location so Ubidots will auto-detect it correctly
    decoded.location = {
      value: 0,
      context: {
        lat: latitude,
        lng: longitude,
      },
    };
  }
  // if (port_format.newSensor) {
  //   decoded.gas = newSensorSchema.decodeValue(bytes, b);
  //   b += newSensorSchema.n_bytes;
  // }

  debugLog(decoded);
  return decoded;
}

/**
 * Template class gatewayData:
 * Used to format the gateway data in getGatewayMetadata().
 */
class gatewayData {
  constructor(
    id,
    eui,
    rssi,
    snr
    // uplink_token
  ) {
    this.id = id;
    this.eui = eui;
    this.rssi = rssi;
    this.snr = snr;
    // this.uplink_token = uplink_token;
    this.location = {
      lat: null,
      lng: null,
    };
  }
}

function getGatewayMetadata(gateways) {
  // get data for the gateway(s)
  let gateways_context = {};
  let gateways_count = 0;
  for (const i in gateways) {
    let gw = gateways[i];
    // create and fill context data for this gateway
    let gw_context = new gatewayData(
      gw["gateway_ids"]["gateway_id"],
      gw["gateway_ids"]["eui"],
      gw["rssi"],
      gw["snr"]
      // gw["uplink_token"]
    );
    // set location info if available
    if ("location" in gw) {
      let location = gw["location"];
      if ("latitude" in location && "longitude" in location) {
        gw_context.location.lat = location["latitude"];
        gw_context.location.lng = location["longitude"];
      }
    }
    gateways_context[gw["gateway_ids"]["gateway_id"]] = gw_context;
    gateways_count++;
  }

  debugLog(gateways_context);
  return [gateways_count, gateways_context];
}

module.exports = { format_payload };
