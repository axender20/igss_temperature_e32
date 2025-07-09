#include "WiFiMonitorTask.h"
#include "esp_log.h"
#include <freertos_delay.h>

static const char* TAG_WIFI_MON = "WiFiMonitor";

void WiFiMonitorTask::begin() {
    xTaskCreate(
        WiFiMonitorTask::taskFunction,
        "wifi_monitor",
        4096,
        this,
        1,
        nullptr
    );
}

void WiFiMonitorTask::taskFunction(void* parameter) {
    const uint32_t checkIntervalMs = 10000;
    uint8_t maxFails = 30;
    uint8_t wifi_fail_count = 0;
    while (true) {
        delay_frms(checkIntervalMs);
        if (WiFi.status() != WL_CONNECTED) {
            ESP_LOGW(TAG_WIFI_MON, "WiFi desconectado, intentando reconectar...");
            WiFi.reconnect();
            wifi_fail_count++;
            if (wifi_fail_count > maxFails) {
                ESP_LOGE(TAG_WIFI_MON, "No se pudo reconectar WiFi despues de 30 intentos");
                wifi_fail_count = 0;
                // ESP.restart();
            }
        } else {
            wifi_fail_count = 0;
        }
    }
}