#include "NtpSyncTask.h"
#include <time.h>
#include <freertos_delay.h>

void NtpSyncTask::begin()
{
    xTaskCreate(
        taskFunction,
        "NtpSyncTask",
        STACK_SIZE,
        this,
        TASK_PRIORITY,
        nullptr);
}

void NtpSyncTask::taskFunction(void *parameter)
{
    const unsigned long delayMs = 15UL * 24UL * 60UL * 60UL * 1000UL;
    while (true)
    {
        delay_frms(delayMs);
        configTime(0, 0, "pool.ntp.org", "time.nist.gov");
        setenv("TZ", "CST6", 1);
        tzset();
        Serial.println("Sincronización NTP realizada.");
    }
}