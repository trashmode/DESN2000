#include <gtest/gtest.h>
#include "measurement_test.h"
// #include "hello_test.h"
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// #include <gtest/gtest.h>

// // TEST(...)
// // TEST_F(...)

// // #if defined(ARDUINO)
// // #include <Arduino.h>
// // #include "measurement_test.h"

// // void setup()
// // {
// //     // should be the same value as for the `test_speed` option in "platformio.ini"
// //     // default value is test_speed=115200
// //     Serial.begin(115200);

// //     ::testing::InitGoogleTest();
// //     // if you plan to use GMock, replace the line above with
// //     // ::testing::InitGoogleMock();
// // }

// // void loop()
// // {
// //   // Run tests
// //   if (RUN_ALL_TESTS())
// //   ;

// //   // sleep for 1 sec
// //   delay(1000);
// // }

// // #else
// #include "measurement_test.h"
// int main(int argc, char **argv)
// {
//     ::testing::InitGoogleTest(&argc, argv);
//     // if you plan to use GMock, replace the line above with
//     // ::testing::InitGoogleMock(&argc, argv);

//     if (RUN_ALL_TESTS())
//     ;

//     // Always return zero-code and allow PlatformIO to parse results
//     return 0;
// }
// // #endif