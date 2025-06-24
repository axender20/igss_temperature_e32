#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_INFO
#include "Arduino.h"
#include "temperature_sensor.h"
#include "audible_alarm.h"
#include "rgb_led.h"

#include "handler_rtemperature.h"

void setup() {
  init_rgb_strip();

  init_handler_rtemperature();
  xTaskCreate(thread_handler_rtemperature, "tsk_tmp", 4096, NULL, 1, NULL);
}

void loop() {

}