#include "measurement.h"

int anologue_to_digital(int anolouge_value) {
    int temp = anolouge_value/5;
    return temp;
}