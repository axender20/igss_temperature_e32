//> Nivel de debug
#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_VERBOSE

#include <Arduino.h>
#include "TempMonitorConfig.h"
#include "SendTempTask.h"
#include "temperature_sensor.h"
#include "handler_rtemperature.h"
#include "NtpSyncTask.h"
#include "esp_log.h"
#include "WiFiMonitorTask.h"

static const char *TAG = "main";

NtpSyncTask ntpTask;
WiFiMonitorTask wifiMonitor;
TempMonitorConfig config;
SendTempTask sendTask;

void setup()
{

    Serial.begin(115200);

    if (!config.begin())
    {
        ESP_LOGE(TAG, "Fallo al configurar WiFi");
        delay(3000);
        ESP.restart();
    }

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

    if (!sendTask.begin(config.getFrecMuestreo(), umbMax, umbMin, config.getAlertsActive()))
    {
        ESP_LOGE(TAG, "Fallo al iniciar tarea de envio");
        ESP.restart();
        delay(3000);
    }

    // tarea de actualizacion horaria por ntp
    ntpTask.begin();

    wifiMonitor.begin();
}

void loop()
{
    vTaskDelay(1);
}