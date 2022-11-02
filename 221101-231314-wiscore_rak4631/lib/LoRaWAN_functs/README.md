# LoRaWAN Functions

A library that collects the configuration and helper functions for using the RAK WisBlock LoRaWAN modules.

## Dependencies

Hardware:

- RAK WisBlock 4630

Software:

- Arduino.h
- [LoRaWan-RAK4630.h](../../#environment-setup)
- [Logging.h](../Logging/)
- [OTAA_keys.h](#otaa-keys)

## Usage

Steps:

1. Include LoRaWAN_functs.h in the main file.
2. Follow the [steps below](#otaa-keys) to add the OTAA keys.
3. Check the LoRaWAN config/parameters at the top of LoRaWAN_functs.h.
4. Initialise the LoRaWAN module in `setup()` with `initLoRaWAN()`.
5. Join the network with `startLoRaWANJoinProcedure()`.
6. Once connected, start sending with `sendLoRaWANFrame()`.

### Example

_Copied from examples\simple_lorawan_example.cpp:_

```c++
#include <Arduino.h>
#include <LoRaWan-RAK4630.h> // Click to get library: https://platformio.org/lib/show/6601/SX126x-Arduino

#include "LoRaWAN_functs.h" /**< Go here to provide the OTAA keys & change the LoRaWAN settings. */
#include "OTAA_keys.h"      /**< Go here to set the OTAA keys (See README). */

// App payload interval value in [ms] = 30 seconds.
const int lorawan_app_interval = 30000;

// Buffer that payload data is placed in.
uint8_t payload_buffer[PAYLOAD_BUFFER_SIZE] = "Hello!";

// Struct that passes the payload data for a LoRaWAN frame.
lmh_app_data_t lorawan_payload = {
    payload_buffer, strlen("Hello!"), 1, 0, 0
};

void setup() {
    // Init LoRaWAN
    if (!initLoRaWAN(OTAA_KEY_APP_EUI, OTAA_KEY_DEV_EUI, OTAA_KEY_APP_KEY)) {
        delay(1000);
        return;
    }

    // Attempt to join the network
    startLoRaWANJoinProcedure();
}

void loop() {
    // every lorawan_app_interval milliseconds check if the device is connected
    delay(lorawan_app_interval);
    if (isLoRaWANConnected()) {
        // send sendLoRaWANFrame will do nothing if not connected anyway, but it's best practice to check
        sendLoRaWANFrame(&lorawan_payload);
    }
}

```

## OTAA Keys

As the OTAA keys are part of the connection security they are not included in this repo.
Instead they must be provided in a file named `OTAA_keys.h` stored in the same directory as your `main.cpp` that defines the:

- Application EUI
- Device EUI (found on the RAK device)
- Application Key

To define the keys create the `OTAA_keys.h` file, `#include` the file in your `main.cpp`, and copy in the below contents.

```cpp
// OTAA Keys are MSB
// Copy the keys from TTS & make sure to match the "0x00, 0x00 ...." formatting.
uint8_t OTAA_KEY_APP_EUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t OTAA_KEY_DEV_EUI[8]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t OTAA_KEY_APP_KEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// Do NOT commit this file to the repo (it should be in .gitignore)
```

Replace the keys with those for the individual device as defined on [TTS](../../../TheThingsStackDocumentation.md#otaa-keys); the device EUI should match the one found on the devices RAK4630 label/QR code.

## LoRaWAN Config/Parameters

At the top of LoRaWAN_functs.h you'll find the LoRaWAN configuration and parameters used in the connection setup.
Feel free to change any of these as required, baring in mind how they effect the connection performance and power consumption.

```c++
static const DeviceClass_t loraClass = CLASS_A;                 /**< Class definition. */
static const LoRaMacRegion_t loraRegion = LORAMAC_REGION_AU915; /**< Region:AU915. */
static const lmh_confirm loraConfirm = LMH_UNCONFIRMED_MSG;     /**< Confirm/unconfirm packet definition. */
#define LORAWAN_JOIN_TRIALS 3                                   /**< Join request reattempts. */
#define PAYLOAD_BUFFER_SIZE 64                                  /**< Data payload buffer size. */
```

Additionally the TX power can optionally be passed to `initLoRaWAN()`, otherwise it defaults to `LORAWAN_DEFAULT_TX_POWER` = `TX_POWER_0`.

Refer to the LoRaWAN specification for further detail.

## Troubleshooting the Connection

First and foremost the forums for [RAK](https://forum.rakwireless.com/) and [TTS](https://www.thethingsnetwork.org/forum/) can be very useful places to debug any issues.

To start debugging I would recommend [enabling the debug logs](../Logging/#usage), and pulling up TTS console; optionally it's also very helpful to have your own gateway to debug the connection.

**Some basic advice:** Check that your [OTAA keys](#otaa-keys) match those on TTS and see if any messages are coming through on the application (and/or gateways) live data streams.
If you're seeing the "Accept join-request" messages on TTS, but you're not getting the "Network Joined!" log then the device may be struggling to receive data. You could try moving closer to/in line of sight of the gateway and/or switching out the antenna for a more powerful one.

If you're not seeing anything on TTS, then you may not be reaching a gateway at all, which is tricky to debug without access to a gateway.
Try uping the transmission power (`LORAWAN_TX_POWER`), and getting closer closer to/in line of sight of the gateway. Also double check that the LoRaWAN (MAC) & Regional Parameters (PHY) versions are compatible with the device and network.

## Suggested Next Steps

The devices are not expecting to receive any downlink messages, and hence currently don't really do anything with them if they were to be received (see `lorawanRXHandler()` in LoRaWAN_functs.cpp). If you'd like to have a back-and-forth connection, you will need to extend the library and implement this in the `lorawanRXHandler()` callback.

Once permanent application modifiable memory is included on the boards (e.g. EEPROM), the devices should begin to store the OTAA credentials instead of completely re-joining the network on reset. It is not good practice to regularly rejoin the network in this fashion as it can clog it up. This isn't too much of any issue at the moment as the devices aren't expected to reset regularly, but if this were to change and/or many more devices were hoping to use the network then it would be advisable. This is why the devices will only make a limited number of attempts (`LORAWAN_JOIN_TRIALS`) to join the network before just stopping until manually reset, as otherwise it would be spamming the network.

The OTAA keys should be unique for each device (as they are on TTS) anf unfortunately they are currently part of the compilation of the device, which makes flashing many devices a pain. This is not essential going forward, but ideally some sort of compilation tool (or other creative solution like Bluetooth, etc.) could be developed to simiplfy this process.
