#ifndef SEND_TEMP_TASK
#define SEND_TEMP_TASK

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class SendTempTask
{
private:
    const char* serverUrl = "http://your-api.com/data";

    bool sendTemperatureData(float temperature);

    static const uint16_t STACK_SIZE = 8192;
    static const UBaseType_t TASK_PRIORITY = 1;

    TaskHandle_t taskHandle;
    int frecuenciaMuestreo;

    static void taskFunction(void* parameter);

public:
    SendTempTask();
    bool begin(int frecMuestreo);
    void stop();
    bool isRunning();
};

#endif