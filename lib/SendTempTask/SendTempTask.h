#ifndef SEND_TEMP_TASK
#define SEND_TEMP_TASK

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "freertos_delay.h"
#include "secrets.h"

class SendTempTask
{
private:
    const char *tableN = "data_device";
    const String endpoint = String(SERVER_URL) + "/rest/v1/" + tableN;
    char deviceId[18];
    bool sendTemperatureData(float temperature);

    static const uint16_t STACK_SIZE = 8192;
    static const UBaseType_t TASK_PRIORITY = 1;

    TaskHandle_t taskHandle;
    int frecuenciaMuestreo;
    float umbMax;
    float umbMin;
    bool alertsActive;

    static void taskFunction(void *parameter);

public:
    SendTempTask();
    bool begin(int frecMuestreo, float umbMaxTemp, float umbMinTemp, bool alertasActivas = false);
    void stop();
    bool isRunning();
};

#endif