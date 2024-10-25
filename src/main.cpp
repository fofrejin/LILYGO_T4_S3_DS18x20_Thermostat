#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <OneWire.h> // DS18S20, DS18B20, DS1822
#include <lv_button.h>

// The default connection is GPIO21
#define DS_DATA   21   // on pin  (a 4.7K resistor is necessary)

LilyGo_Class amoled;
OneWire  ds(DS_DATA);
lv_obj_t *label1;

void display_init(void)
{
    bool rslt = false;

    // Begin LilyGo  1.47 Inch AMOLED board class
    //rslt = amoled.beginAMOLED_147();


    // Begin LilyGo  1.91 Inch AMOLED board class
    //rslt =  amoled.beginAMOLED_191();

    // Begin LilyGo  2.41 Inch AMOLED board class
    rslt =  amoled.beginAMOLED_241();

    // Automatically determine the access device
    // rslt = amoled.begin();

    if (!rslt) {
        while (1) {
            Serial.println("The board model cannot be detected, please raise the Core Debug Level to an error");
            delay(1000);
        }
    }
}

void lv_init_custom(void)
{
    // Register lvgl helper
    beginLvglHelper(amoled);

    // Dispay temperature label
    label1 = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_48, 0);
    lv_label_set_text(label1, "0°C");
    lv_obj_center(label1);

    lv_display_button();
}

float get_temperature_celsius(void)
{
    byte i;
    byte present = 0;
    byte type_s;
    byte data[9];
    byte addr[8];
    float celsius;

    if ( !ds.search(addr)) {
        Serial.println("No more addresses.");
        Serial.println();
        ds.reset_search();
        delay(250);
        return -125.00;
    }

    Serial.print("ROM =");
    for ( i = 0; i < 8; i++) {
        Serial.write(' ');
        Serial.print(addr[i], HEX);
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return -126.00;
    }
    Serial.println();

    // the first ROM byte indicates which chip
    switch (addr[0]) {
    case 0x10:
        Serial.println("  Chip = DS18S20");  // or old DS1820
        type_s = 1;
        break;
    case 0x28:
        Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
    case 0x22:
        Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
    default:
        Serial.println("Device is not a DS18x20 family device.");
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

    Serial.print("  Data = ");
    Serial.print(present, HEX);
    Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
        data[i] = ds.read();
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.print(" CRC=");
    Serial.print(OneWire::crc8(data, 8), HEX);
    Serial.println();

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10) {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    } else {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
        //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");

    return celsius;
}

void display_temperature(void)
{
    float temperature = get_temperature_celsius();
    if (temperature > -125.00) {
      lv_label_set_text_fmt(label1, "%.2f°C", temperature);
      lv_obj_center(label1);
    }
}

void taskOne( void * parameter )
{
    while(1) {
        lv_task_handler();
    } 
}

void taskTwo( void * parameter)
{
    while(1) {
      display_temperature();
    }
}

void tasks_init(void)
{
    xTaskCreate(
                      taskOne,          /* Task function. */
                      "TaskOne",        /* String with name of task. */
                      10000,            /* Stack size in bytes. */
                      NULL,             /* Parameter passed as input of the task */
                      1,                /* Priority of the task. */
                      NULL);            /* Task handle. */

    xTaskCreate(
                    taskTwo,          /* Task function. */
                    "TaskTwo",        /* String with name of task. */
                    10000,            /* Stack size in bytes. */
                    NULL,             /* Parameter passed as input of the task */
                    1,                /* Priority of the task. */
                    NULL);            /* Task handle. */
}

//----------------------------------------------------------------------------------------------------------------------
void setup(void)
{
    Serial.begin(115200);
    display_init();
    lv_init_custom();
    tasks_init();
}

void loop(void)
{
    delay(1000);
}