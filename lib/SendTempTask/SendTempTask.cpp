#include "SendTempTask.h"

SendTempTask::SendTempTask() : taskHandle(NULL),
                               frecuenciaMuestreo(0) {}

void SendTempTask::taskFunction(void *parameter)
{
    SendTempTask *task = (SendTempTask *)parameter;
    const TickType_t delayTicks = pdMS_TO_TICKS((uint32_t)task->frecuenciaMuestreo * 1000);

    while (true)
    {
        // lectura de temperatura
        float temperature = random(20, 30) + (random(0, 100) / 100.0);
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

        vTaskDelay(delayTicks);
    }
}

bool SendTempTask::sendTemperatureData(float temperature)
{
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Authorization", "Bearer your-api-key");
    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["t"] = temperature;
    // doc["device_id"] = "ESP32_TEMP_001";

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

bool SendTempTask::begin(int frecMuestreo)
{
    if (taskHandle != NULL)
    {
        return false;
    }

    frecuenciaMuestreo = frecMuestreo;

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