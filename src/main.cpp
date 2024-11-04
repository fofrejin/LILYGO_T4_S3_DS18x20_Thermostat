#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <lv_objects.h>
#include <sensors.h>
#include <weather.h>

LilyGo_Class amoled;

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

    if(!rslt) {
        while(1) {
            Serial.println("The board model cannot be detected, please raise the Core Debug Level to an error");
            delay(1000);
        }
    }
}

void lv_init_custom(void)
{
    // Register lvgl helper
    beginLvglHelper(amoled);

    lv_init_labels();
    lv_display_button();
}

void display_temperature(void)
{
    float temperature = get_temperature_celsius();
    if(temperature > -125.00) {
        lv_set_current_temperature(temperature);
    }
}

void taskOne(void * parameter)
{
    while(1) {
        lv_task_handler();
    }
}

void taskTwo(void * parameter)
{
    while(1) {
        display_temperature();
    }
}

void taskThree(void * parameter)
{
    while(1) {
        weather_data_process();
        vTaskDelay(1800000); // 30 min
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

    xTaskCreate(
        taskThree,          /* Task function. */
        "taskThree",        /* String with name of task. */
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
    pinMode(18, OUTPUT);
    digitalWrite(18, HIGH);
    setup_connection();
}

void loop(void)
{
    delay(1000);
}