#pragma once
#include <Arduino.h>

class NtpSyncTask {
public:
    NtpSyncTask(){};
    void begin();

private:
    static void taskFunction(void* parameter);
    static const uint16_t STACK_SIZE = 4096;
    static const UBaseType_t TASK_PRIORITY = 1;
};