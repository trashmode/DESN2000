// #include <gtest/gtest.h>
#include "../src/measurement.cc"

TEST(MeasurementTest, BasicAssertion) {

    EXPECT_EQ(anologue_to_digital(5), 1);

}