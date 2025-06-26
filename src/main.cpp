#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_INFO
#include <Arduino.h>
#include "TempMonitorConfig.h"
#include "SendTempTask.h"
#include "temperature_sensor.h"
#include "handler_rtemperature.h"
#include "shared_temperature_status.h"

#if defined(ESP32_S3_DEVKITM_1)
  #define CONFIG_BUTTON_PIN 37
#elif defined(ESP_CAM)
  #define CONFIG_BUTTON_PIN 15
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
    delay(100);
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

    init_handler_rtemperature();

    xTaskCreate(
        thread_handler_rtemperature,
        "task_read_temperature",
        4096,
        NULL,
        1,
        NULL);

    sendTask.setReadTemperatureFunction([](){
        // return random(20, 30) + (random(0, 100) / 100.0);
        return sh_temperarute_status.get_average();
    });
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