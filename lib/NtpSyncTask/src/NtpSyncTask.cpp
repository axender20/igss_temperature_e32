//> Nivel de debug
#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_VERBOSE

#include "NtpSyncTask.h"
#include <time.h>
#include <freertos_delay.h>
#include "esp_log.h"

static const char *TAG = "ntptask";

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
        ESP_LOGI(TAG, "Sincronización NTP realizada");
    }
}