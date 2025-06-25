//> Nivel de debug 
#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_INFO

#include "handler_rtemperature.h"
#include "Arduino.h"
#include "esp_log.h"
#include "temperature_sensor.h"
#include "shared_temperature_status.h"
#include "freertos_delay.h"
#include "rgb_led.h"
#include "audible_alarm.h"

static const char* TAG = "rtemp";

float h_limit = 30;
float l_limit = 15;

void task_handler_rtemperature();

void thread_handler_rtemperature(void* parametres) {
  //? Intervalo del thread (1 tick)
  static TickType_t lw_thread_rtemp, interval_thread_rtemp = 1000;

  BaseType_t delayed;
  lw_thread_rtemp = xTaskGetTickCount();
  while (1) {
    task_handler_rtemperature();
    delayed = xTaskDelayUntil(&lw_thread_rtemp, interval_thread_rtemp);
    if (delayed == pdFALSE) {
      lw_thread_rtemp = xTaskGetTickCount();
    }
  }
  vTaskDelete(NULL);
}

void init_handler_rtemperature() {
  temperature_sensor::init();
  audible_alarm::init();
  ESP_LOGI(TAG, "Handler temperature initializate");
}

bool check_temperature_alert(float actll_temp, float high_limit, float low_limit);
void show_alerts();
void disable_alerts();

void task_handler_rtemperature() {
  //? Read temperature 
  float temperature = 0.0f, h;
  bool r_temp = temperature_sensor::get_data(temperature, h);
  if (!r_temp) {
    //TODO: Realizar gestion de errores, no lectura del sensor
    ESP_LOGE(TAG, "Error read temperature data");
  }

  //? Save to shared data 
  sh_temperarute_status.add_average(temperature);
  ESP_LOGD(TAG, "Temp: %F", temperature);

  static bool last_alarm = false;
  bool alarm = check_temperature_alert(temperature, h_limit, l_limit);

  if (alarm) {
    //? Enable alarms and modify shared data
    if (!last_alarm) {
      ESP_LOGI(TAG, "Enable temperature alerts");

      //? Primera vez modificar alarma
      sh_temperarute_status.set_alarm(true);
    }
    sh_temperarute_status.set_raw(temperature);
    show_alerts();
  }
  else if (last_alarm && !alarm) {
    ESP_LOGI(TAG, "Disable temperature alerts");

    //? Disable alarms (once)
    sh_temperarute_status.set_alarm(false);
    disable_alerts();
  }
  last_alarm = alarm;

  delay_frms(100);
}

bool check_temperature_alert(float temperature, float high_limit, float low_limit) {
  //> Initial value for temperature alarm
  static bool temperature_alert_active = false;

  float percent = 10; //Porcentaje de limites 
  float factor = percent / 100.0;  // => 0.10

  float high_set = high_limit;
  float high_clear = high_set - (high_set * factor);

  float low_set = low_limit;
  float low_clear = low_limit + (low_set * factor);

  // Verifica si debe activarse la alerta
  if (!temperature_alert_active) {
    if (temperature >= high_set || temperature <= low_set) {
      temperature_alert_active = true;
    }
  }
  // Verifica si debe desactivarse la alerta
  else {
    if (temperature <= high_clear && temperature >= low_clear) {
      temperature_alert_active = false;
    }
  }

  return temperature_alert_active;
}

static bool alert_state = false;
static ulong last_alert_toggle = 0u;

void show_alerts() {
  //> Blink led error and buzzer  
  unsigned long current_time = millis();
  const uint32_t ALERT_BLINK_INTERVAL = 500u;
  if (current_time - last_alert_toggle >= ALERT_BLINK_INTERVAL) {
    alert_state = !alert_state;
    last_alert_toggle = current_time;

    if (alert_state) {
      wrgb_2.switch_color(255, 0, 0);
      audible_alarm::on();
    }
    else {
      wrgb_2.off();
      audible_alarm::off();
    }
  }
}

void disable_alerts() {
  //> Disable led, buzzer
  if (alert_state) {
    wrgb_1.off();
    audible_alarm::off();
    last_alert_toggle = 0;
  }
}
