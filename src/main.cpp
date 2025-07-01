#include <Arduino.h>
#include "TempMonitorConfig.h"
#include "SendTempTask.h"
#include "temperature_sensor.h"
#include "handler_rtemperature.h"
// #include <time.h>

// void printCurrentTime() {
//     time_t now = time(nullptr);
//     struct tm timeinfo;
//     localtime_r(&now, &timeinfo);
//     char buffer[30];
//     strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
//     Serial.print("Hora actual: ");
//     Serial.println(buffer);
// }

#if defined(ESP32_S3_DEVKITM_1)
#define CONFIG_BUTTON_PIN 37
#elif defined(ESP_CAM)
#define CONFIG_BUTTON_PIN 13
#endif

TempMonitorConfig config;
SendTempTask sendTask;

void IRAM_ATTR handleButtonPress()
{
    ESP.restart();
}

void setup()
{
    pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
    delay(1000);
    bool needConfig = (digitalRead(CONFIG_BUTTON_PIN) == LOW);

    Serial.begin(115200);

    if (!config.begin(needConfig))
    {
        Serial.println("Failed to configure WiFi");
        delay(3000);
        ESP.restart();
    }
    // fin portal de configuracion wifi

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

    attachInterrupt(digitalPinToInterrupt(CONFIG_BUTTON_PIN), handleButtonPress, FALLING);
}

void loop()
{
    vTaskDelay(1);
}