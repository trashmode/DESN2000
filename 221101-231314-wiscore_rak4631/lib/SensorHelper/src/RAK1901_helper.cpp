#include "RAK1901_helper.h"

bool RAK1901::init(void) {
    Wire.begin(); // The default settings use Wire (default Arduino I2C port).
    // Wire.setClock(400000); // I2C speed is global for that bus, so using 400kHz or 100kHz is recommended.

    // SHTC3 functions return value of type "SHTC3_Status_TypeDef".
    return (begin() == SHTC3_Status_Nominal);
}

bool RAK1901::dataReady(void) {
    update();
    // SHTC3 functions return value of type "SHTC3_Status_TypeDef".
    return (lastStatus == SHTC3_Status_Nominal);
}