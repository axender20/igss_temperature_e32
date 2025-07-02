#include <Arduino.h>
#include "TempMonitorConfig.h"
#include "SendTempTask.h"
#include "temperature_sensor.h"
#include "handler_rtemperature.h"
#include "NtpSyncTask.h"

NtpSyncTask ntpTask;

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
    
    const float umbMax = config.getAlertsActive() ? config.getUmbMax() : 150.0f;
    const float umbMin = config.getAlertsActive() ? config.getUmbMin() : -40.0f;
    init_handler_rtemperature(umbMax, umbMin);

    xTaskCreate(
        thread_handler_rtemperature,
        "task_read_temperature",
        4096,
        NULL,
        1,
        NULL);

    if (!sendTask.begin(config.getFrecMuestreo(), config.getAlertsActive()))
    {
        Serial.print("Fallo al iniciar tarea de envio");
        ESP.restart();
        delay(3000);
    }

    // tarea de actualizacion horaria por ntp
    ntpTask.begin();
}

void loop()
{
    vTaskDelay(1);
}