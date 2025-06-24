#include <Arduino.h>
#include "TempMonitorConfig.h"
#include "SendTempTask.h"

#define CONFIG_BUTTON_PIN 15

TempMonitorConfig config;
SendTempTask sendTask;

void IRAM_ATTR handleButtonPress()
{
    ESP.restart();
}

void setup()
{
    pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
    delay(100);
    bool needConfig = (digitalRead(CONFIG_BUTTON_PIN) == LOW);
    
    Serial.begin(115200);

    if (!config.begin(needConfig))
    {
        Serial.println("Failed to configure WiFi");
        delay(3000);
        ESP.restart();
    }

    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (!sendTask.begin(config.getFrecMuestreo()))
    {
        Serial.print("Fallo al iniciar tarea de envio");
        ESP.restart();
        delay(3000);
    }

    attachInterrupt(digitalPinToInterrupt(CONFIG_BUTTON_PIN), handleButtonPress, FALLING);
}

void loop()
{
    vTaskDelay(1);
}