#include "freertos_delay.h"
#include "Arduino.h"
#include <freertos/task.h>

#define TICKS_PER_MS configTICK_RATE_HZ / 1000

bool isFreeRtosDelay() {
  return (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING && xTaskGetCurrentTaskHandle() != nullptr);
}

void delay_frms(uint32_t _time_ms) {
  if (!(_time_ms > 0)) return;

  if (isFreeRtosDelay()) {
    //? FreeRtos delay
    TickType_t last_count_get = xTaskGetTickCount();
    TickType_t ticks = _time_ms * TICKS_PER_MS;
    vTaskDelayUntil(&last_count_get, ticks);
  }
  else {
    //? Core delay
    delay(_time_ms);
  }
}