#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_ERROR
#include "Arduino.h"
#include "handler_battery.h"
#include "handler_rtemperature.h"
#include "rgb_led.h"
void setup() {
  init_rgb_strip();
  init_handler_rtemperature();
  init_hanlder_battery();

  xTaskCreate(thread_handler_rtemperature, "tsk_tmp", 4096, NULL, 1, NULL);
  xTaskCreate(thread_handler_battery, "tsk_bat", 4096, NULL, 1, NULL);
}

void loop() {
}