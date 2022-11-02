/**
 * @file LoRaWAN_functs.h
 * @author Kalina Knight
 * @brief LoRaWAN functions and any associated variables.
 * Feel free to change the settings of the LoRaWAN connection.
 * The OTAA keys are defined locally (not remotely on GitHub) in a separate header file; see the README for further
 * explanantion.
 *
 * @version 0.1
 * @date 2021-08-24
 *
 * @copyright (c) 2021 Kalina Knight - MIT License
 */

#include <LoRaWan-RAK4630.h>

#include "Logging.h"

// LoRaWAN Config/Default Parameters - feel free to change these defaults to whatever suits the project
static const DeviceClass_t loraClass = CLASS_A;                 /**< Class definition. */
static const LoRaMacRegion_t loraRegion = LORAMAC_REGION_AU915; /**< Region:AU915. */
static const lmh_confirm loraConfirm = LMH_UNCONFIRMED_MSG;     /**< Confirm/unconfirm packet definition. */
#define LORAWAN_JOIN_TRIALS 3                                   /**< Join request reattempts. */
#define PAYLOAD_BUFFER_SIZE 64                                  /**< Data payload buffer size. */

/**
 * @brief Initialise LoRaWAN.
 * @param appEUI    OTAA key app EUI.
 * @param deviceEUI OTAA key device EUI.
 * @param appKey    OTAA key app key.
 * @param tx_power  TX power setting. (Defaults to LORAWAN_DEFAULT_TX_POWER). TX_POWER_0 - TX_POWER_10 valid for AU915.
 * @return True if successful, false if not.
 */
bool initLoRaWAN(uint8_t *appEUI, uint8_t *deviceEUI, uint8_t *appKey, uint8_t tx_power = LORAWAN_DEFAULT_TX_POWER);

/**
 * @brief Initialise LoRaWAN.
 * @param timer     Timer that will be started once network has been successfully joined.
 * @param appEUI    OTAA key app EUI.
 * @param deviceEUI OTAA key device EUI.
 * @param appKey    OTAA key app key.
 * @param tx_power  TX power setting. (Defaults to LORAWAN_DEFAULT_TX_POWER). TX_POWER_0 - TX_POWER_10 valid for AU915.
 * @return True if successful, false if not.
 */
bool initLoRaWAN(SoftwareTimer *timer, uint8_t *appEUI, uint8_t *deviceEUI, uint8_t *appKey, uint8_t tx_power = LORAWAN_DEFAULT_TX_POWER);

/**
 * @brief Attempt to join the LoRaWAN network.
 * Once connected the joined callback set in initLoRaWAN() will be called.
 */
inline void startLoRaWANJoinProcedure(void) {
    lmh_join();
};

/**
 * @brief Sends a frame with the data provided.
 * @param lora_app_data Data to be sent.
 */
void sendLoRaWANFrame(lmh_app_data_t *lora_app_data);

/**
 * @brief Gets the status of the current LoRaWAN connection.
 * @return True if connected, false if not.
 */
inline bool isLoRaWANConnected(void) {
    return (lmh_join_status_get() == LMH_SET);
};

/**
 * @brief Sets the class to loraClass.
 * @return True if successful, false if not.
 */
inline bool setLoRaWANClass(void) {
    return (lmh_class_request(loraClass) == LMH_SUCCESS);
};
