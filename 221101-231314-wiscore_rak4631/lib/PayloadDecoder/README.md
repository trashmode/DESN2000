# Payload Decoder for Ubidots

This decoder function is part of the [Ubidots-TTS Plugin](../tts-plugin); it converts the payload received from TTS into Ubidots digestible data.

## Data Extracted from TTS Payload

[Payloads sent by TTS](https://www.thethingsindustries.com/docs/reference/data-formats/#uplink-messages) include a lot of metadata about the device, gateway and app payload being sent.

The decoder primarily extracts the app payload from the `frm_payload` field that contains the sensor data that has been [encoded according the port schema](#port-&-sensor-schema) specified by `f_port`. The extracted sensor data is formatted to match [Ubidots](https://ubidots.com/docs/hw/?language=Arduino#how-ubidots-works) using `received_at` as the timestamp for each dot.

Optionally the decoder also collects and bundles up the gateway data into the "gateways" variable's context data. This can be toggled on and off by setting `COLLECT_GATEWAY_METADATA` at the top of the code.

## Port & Sensor Schema

The port & sensor schema's in the decoder are those defined and [explained](../../WisBlockFirmware/lib/SensorHelper/#lorawan-ports) in the firmware library SensorHelper. Essentially the port number (and it's associated schema) indicate what is in the payload and how to interpret it.

The implementation of the port schema's in the decoder mirror the implementation in the firmware as closely as makes sense in JavaScript to simplify understanding and modifying the schema.

### Adding a New Port or Sensor Instructions

First follow the [instructions](../../WisBlockFirmware/lib/SensorHelper/#new-port-or-sensor-instructions) in the SensorHelper library to add to the firmware. The firmware is the source of truth for the schemas; this decoder is just the inverse on this side.

Then follow the examples to mirror the new sensor and/or port schemas in the decoder: `newSensorSchema` and/or `PORTX`. Make sure to also update `decodePayload()`.

## Editing Tip

It is significantly easier to edit this code in VS Code and then copy and paste it into the decoding function than editting it directly in Ubidots. Plus if you're using any version control you can also track the changes made over time.
