#pragma once
#include <Arduino.h>
#include <WiFi.h>

class WiFiMonitorTask {
public:
    WiFiMonitorTask(){};
    void begin();

private:
    static void taskFunction(void* parameter);
};