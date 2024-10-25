#include <OneWire.h> // DS18S20, DS18B20, DS1822

#define DEBUG_PRINTLN(...) // Serial.println(__VA_ARGS__)
#define DEBUG_PRINT(...) // Serial.print(__VA_ARGS__)

// The default connection is GPIO21
#define DS_DATA   21   // on pin  (a 4.7K resistor is necessary)

OneWire  ds(DS_DATA);

float get_temperature_celsius(void)
{
    byte i;
    byte present = 0;
    byte type_s;
    byte data[9];
    byte addr[8];
    float celsius;

    if(!ds.search(addr)) {
        DEBUG_PRINTLN("No more addresses.");
        DEBUG_PRINTLN();
        ds.reset_search();
        delay(250);
        return -125.00;
    }

    DEBUG_PRINT("ROM =");
    for(i = 0; i < 8; i++) {
        Serial.write(' ');
        DEBUG_PRINT(addr[i], HEX);
    }

    if(OneWire::crc8(addr, 7) != addr[7]) {
        DEBUG_PRINTLN("CRC is not valid!");
        return -126.00;
    }
    DEBUG_PRINTLN();

    // the first ROM byte indicates which chip
    switch(addr[0]) {
        case 0x10:
            DEBUG_PRINTLN("  Chip = DS18S20");  // or old DS1820
            type_s = 1;
            break;
        case 0x28:
            DEBUG_PRINTLN("  Chip = DS18B20");
            type_s = 0;
            break;
        case 0x22:
            DEBUG_PRINTLN("  Chip = DS1822");
            type_s = 0;
            break;
        default:
            DEBUG_PRINTLN("Device is not a DS18x20 family device.");
            return -127.00;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end

    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);         // Read Scratchpad

    DEBUG_PRINT("  Data = ");
    DEBUG_PRINT(present, HEX);
    DEBUG_PRINT(" ");
    for(i = 0; i < 9; i++) {             // we need 9 bytes
        data[i] = ds.read();
        DEBUG_PRINT(data[i], HEX);
        DEBUG_PRINT(" ");
    }
    DEBUG_PRINT(" CRC=");
    DEBUG_PRINT(OneWire::crc8(data, 8), HEX);
    DEBUG_PRINTLN();

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if(type_s) {
        raw = raw << 3; // 9 bit resolution default
        if(data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    }
    else {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if(cfg == 0x00) raw = raw & ~7;   // 9 bit resolution, 93.75 ms
        else if(cfg == 0x20) raw = raw & ~3;  // 10 bit res, 187.5 ms
        else if(cfg == 0x40) raw = raw & ~1;  // 11 bit res, 375 ms
        //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    DEBUG_PRINT("  Temperature = ");
    DEBUG_PRINT(celsius);
    DEBUG_PRINT(" Celsius, ");

    return celsius;
}