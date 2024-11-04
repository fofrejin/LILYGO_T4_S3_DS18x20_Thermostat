#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>
#include <ArduinoJson.h>

#include "lv_objects.h"
#include "weather_config.h"

#define WEATHER_HOST WEATHER_CONFIG_HOST // from weather_config.h
// Example: "http://dataservice.accuweather.com/currentconditions/v1/123456?apikey=dwD1jgGFycfuEFdCVSt531878dbDeGDED&language=en-us&details=false"

#define WIFI_CONNECTION_WAIT_DELAY_MS 500

float temperature_outside = -127;

WiFiMulti wifiMulti;

char ssid[] = WIFI_SSID; // from weather_config.h
char pass[] = WIFI_PASS; // from weather_config.h

void setup_connection(void)
{
    wifiMulti.addAP(ssid, pass);
}

String getWeatherData(void)
{
    String payload;
    while((wifiMulti.run() != WL_CONNECTED)) {
        Serial.print("WiFi not connected\n");
        vTaskDelay(WIFI_CONNECTION_WAIT_DELAY_MS);
    }

    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    if(http.begin(WEATHER_HOST)) {    // HTTP
        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                payload = http.getString();
                Serial.println(payload);
            }
        }
        else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }
    else {
        Serial.printf("[HTTP} Unable to connect\n");
    }
    return payload;
}

int parseWeatherData(String payload)
{
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, payload);
    if(error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        temperature_outside = NAN;
        lv_set_outside_temperature(temperature_outside);
        return -1;
    }

    JsonArray array = doc.as<JsonArray>();
    temperature_outside = array[0]["Temperature"]["Metric"]["Value"];
    Serial.print("accuweather: ");
    Serial.print("temperature_outside: ");
    Serial.println(temperature_outside);
    lv_set_outside_temperature(temperature_outside);
    char utc_offset_str[50];
    strcpy(utc_offset_str, array[0]["LocalObservationDateTime"]);
    Serial.print("utc_offset_str: ");
    Serial.println(utc_offset_str);
    return 0;
}

int weather_data_process(void)
{
    return parseWeatherData(getWeatherData());
}
