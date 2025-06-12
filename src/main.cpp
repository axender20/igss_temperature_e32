#include <Arduino.h>
#include "TempMonitorConfig.h"
#include "SendTempTask.h"
TempMonitorConfig config;
SendTempTask sendTask;

void setup()
{
    Serial.begin(115200);

    if (!config.begin())
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
}

void loop()
{
    vTaskDelay(1);
}