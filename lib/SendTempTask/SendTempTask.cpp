#include "SendTempTask.h"
#include <shared_temperature_status.h>
#include <EmailSender.h>

SendTempTask::SendTempTask() : taskHandle(NULL),
                               frecuenciaMuestreo(0),
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
    static unsigned long lastEmailSent = 0;
    const unsigned long emailCooldown = 180000; // 180 segundos (3 minutos) en ms
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
            
            Serial.printf("Temperatura: %.2f°C\n", temperature);

            if (task->sendTemperatureData(temperature))
            {
                Serial.println("Datos enviados correctamente");
            }
            else
            {
                Serial.println("Error al enviar datos");
            }

            Serial.printf("Stack libre: %d words\n", uxTaskGetStackHighWaterMark(NULL));
            Serial.println();
            Serial.println();

            secondsToPassFM = 0;
        }
        if (_alertsActive && sh_temperarute_status.get_alarm())
        {
            unsigned long now = millis();
            if (now - lastEmailSent >= emailCooldown)
            {
                float raw_temp = sh_temperarute_status.get_raw();
                String subject = "¡Alerta de temperatura!";
                String body = "Se ha detectado una temperatura fuera de rango.\n";
                body += "Temperatura actual: ";
                body += String(raw_temp, 2);
                body += " °C\n";
                body += "MAC del dispositivo: ";
                body += task->deviceId;
                body += "\n";
                body += "Fecha/hora: #esp_mail_current_time\n";
                EmailSender::getInstance().sendMail(subject, body);
                lastEmailSent = now;
            }
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
        Serial.println("Error: No autorizado");
        break;
    case HTTP_CODE_NOT_FOUND: // 404
        Serial.println("Error: API endpoint no encontrado");
        break;
    default:
        Serial.printf("Error: Código no manejado: %d\n", httpResponseCode);
        break;
    }

    if (success)
    {
        Serial.printf("HTTP Response code: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println(response);
    }
    else
    {
        Serial.printf("Error code: %d\n", httpResponseCode);
    }
    http.end();
    return success;
}

bool SendTempTask::begin(int frecMuestreo, bool alertasActivas)
{
    if (taskHandle != NULL)
    {
        return false;
    }

    frecuenciaMuestreo = frecMuestreo;
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