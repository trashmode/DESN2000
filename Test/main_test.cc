#include <gtest/gtest.h>
#include "function_test.h"
#include "hello_test.h"
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}