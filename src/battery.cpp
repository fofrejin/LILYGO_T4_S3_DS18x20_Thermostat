#include <Wire.h>

#include "lv_objects.h"

const int SY6970_ADDRESS = 0x6A;  // Default I²C address for SY6970
const int BATTERY_VOLTAGE_REG = 0x0E;  // Register for battery voltage

float battery_voltage_get()
{
    Wire.beginTransmission(SY6970_ADDRESS);
    Wire.write(BATTERY_VOLTAGE_REG);
    Wire.endTransmission(false);

    Wire.requestFrom(SY6970_ADDRESS, 1);
    if(Wire.available() == 1) {
        int rawVoltage = Wire.read();

        float batteryVoltage = 2.304 + rawVoltage * 0.020;
        return batteryVoltage;
    }
    else {
        return -1.0;
    }
}

void battery_voltage_display()
{
    float batteryVoltage = battery_voltage_get();
    if(batteryVoltage >= 0) {
        lv_set_battery_voltage(batteryVoltage);
    }
}
