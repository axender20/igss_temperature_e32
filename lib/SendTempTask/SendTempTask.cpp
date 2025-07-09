//> Nivel de debug
#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_VERBOSE
#include "SendTempTask.h"
#include <shared_temperature_status.h>
#include <EmailSender.h>
#include "esp_log.h"

static const char *TAG = "sndtmptask";

SendTempTask::SendTempTask() : taskHandle(NULL),
                               frecuenciaMuestreo(0),
                               umbMax(0),
                               umbMin(0),
                               alertsActive(false)
{
    uint8_t mac[6];
    WiFi.macAddress(mac);
    snprintf(deviceId, sizeof(deviceId), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void SendTempTask::taskFunction(void *parameter)
{
    SendTempTask *task = (SendTempTask *)parameter;
    const uint32_t delayMsSeg = 1000;
    const int delayFrecM = (int)task->frecuenciaMuestreo;
    int secondsToPassFM = 0;
    static unsigned long lastEmailSent = 180000;
    static unsigned long alarmStartTime = 0;
    static bool alarmPersitActive = false;
    const unsigned long emailCooldown = 180000; // 180 segundos (3 minutos) en ms
    const unsigned long alarmHoldTime = 15000;  // 15 segundos
    const bool _alertsActive = task->alertsActive;
    while (true)
    {
        delay_frms(delayMsSeg);
        secondsToPassFM++;

        if (secondsToPassFM >= delayFrecM)
        {
            // lectura de temperatura
            // float temperature = random(20, 30) + (random(0, 100) / 100.0);
            float temperature = sh_temperarute_status.get_average();
            temperature = roundf(temperature * 100.0f) / 100.0f;

            task->sendTemperatureData(temperature);
            // Serial.printf("Stack libre: %d words\n", uxTaskGetStackHighWaterMark(NULL));
            // Serial.println();
            // Serial.println();

            secondsToPassFM = 0;
        }
        if (_alertsActive && sh_temperarute_status.get_alarm())
        {
            unsigned long now = millis();
            if (!alarmPersitActive)
            {
                alarmPersitActive = true;
                alarmStartTime = now;
            }
            if ((now - alarmStartTime >= alarmHoldTime) &&
                (now - lastEmailSent >= emailCooldown))
            {
                float raw_temp = sh_temperarute_status.get_raw();
                String subject = "¡Alerta de temperatura!";
                String body = "Se ha detectado una temperatura fuera de rango.\n";
                body += "Temperatura actual: ";
                body += String(raw_temp, 2);
                body += " °C\n";
                body += "Umbrales de temperatura configurados\n: ";
                body += " Max:";
                body += String((float)task->umbMax, 2);
                body += " °C\n";
                body += " Min:";
                body += String((float)task->umbMin, 2);
                body += " °C\n";
                body += "MAC del dispositivo: ";
                body += task->deviceId;
                body += "\n";
                body += "Fecha/hora: #esp_mail_current_time\n";
                body += "\n";
                // body += "Mayor información visite:\n";
                // body += "https://cloud.temperatura.iot.com.gt\n";
                EmailSender::getInstance().sendMail(subject, body);
                lastEmailSent = now;
                task->sendTemperatureData(raw_temp);
            }
        }
        else
        {
            alarmPersitActive = false;
            alarmStartTime = 0;
        }
    }
}

bool SendTempTask::sendTemperatureData(float temperature)
{
    HTTPClient http;
    http.begin(endpoint);
    String authHeader = "Bearer ";
    authHeader += API_KEY;
    http.addHeader("Authorization", authHeader);
    http.addHeader("apiKey", API_KEY);
    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["temperature"] = temperature;
    doc["device_id"] = deviceId;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);
    bool success = false;
    switch (httpResponseCode)
    {
    case HTTP_CODE_OK:      // 200
    case HTTP_CODE_CREATED: // 201
        success = true;
        break;
    case HTTP_CODE_UNAUTHORIZED: // 401
        ESP_LOGE(TAG, "Error: No autorizado (401)");
        break;
    case HTTP_CODE_NOT_FOUND: // 404
        ESP_LOGE(TAG, "Error: API endpoint no encontrado (404)");
        break;
    case HTTP_CODE_CONFLICT:
        ESP_LOGE(TAG, "Error: Conflicto de datos (409)");
        break;
    default:
        ESP_LOGE(TAG, "Error: Código HTTP no manejado: %d", httpResponseCode);
        break;
    }

    if (success)
    {
        ESP_LOGV(TAG, "Codigo de respuesta HTTP: %d", httpResponseCode);
    }
    else
    {
        ESP_LOGE(TAG, "Error. Código HTTP: %d", httpResponseCode);
    }
    http.end();
    return success;
}

bool SendTempTask::begin(int frecMuestreo, float umbMaxTemp, float umbMinTemp, bool alertasActivas)
{
    if (taskHandle != NULL)
    {
        return false;
    }

    frecuenciaMuestreo = frecMuestreo;
    umbMax = umbMaxTemp;
    umbMin = umbMinTemp;
    alertsActive = alertasActivas;

    BaseType_t res = xTaskCreate(
        taskFunction,
        "SendTempTask",
        STACK_SIZE,
        this,
        TASK_PRIORITY,
        &taskHandle);

    return (res == pdPASS);
}

void SendTempTask::stop()
{
    if (taskHandle != NULL)
    {
        vTaskDelete(taskHandle);
        taskHandle = NULL;
    }
}

bool SendTempTask::isRunning()
{
    return taskHandle != NULL;
}