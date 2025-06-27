//> Nivel de debug 
#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_INFO

#include "handler_battery.h"
#include "Arduino.h"
#include "battery_monitor.h"
#include "shared_battery_status.h"
#include "shared_temperature_status.h"
#include "rgb_led.h"

static const char* TAG = "batt";

static const uint8_t battery_low_level = 80;
static RGBLed* led_battery;

static void task_hanlder_battery();
void thread_handler_battery(void* parametres) {
  //? Intervalo del thread (1 ticks)
  static TickType_t lw_thread_batt, interval_thread_batt = 1;

  BaseType_t delayed;
  lw_thread_batt = xTaskGetTickCount();
  while (1) {
    task_hanlder_battery();
    delayed = xTaskDelayUntil(&lw_thread_batt, interval_thread_batt);
    if (delayed == pdFALSE) {
      lw_thread_batt = xTaskGetTickCount();
    }
  }
  vTaskDelete(NULL);
}

static bool enable_alarm = false;
static void show_alarms();

void init_hanlder_battery() {
  sh_battery_status.init();
  battery_monitor::init();
  led_battery = &wrgb_2;
  ESP_LOGI(TAG, " Handler battery initializate");
}


static void task_hanlder_battery() {
  battery_monitor::update();

  show_alarms();
  //> 5 Segundos por tick
  const uint32_t tick_time = 2000;

  bool is_charging = battery_monitor::is_charging();
  sh_battery_status.set_charge(is_charging);

  static ulong tick_millis = 0;
  if (millis() - tick_millis >= tick_time) {
    uint8_t current_level = battery_monitor::get_blevel();
    sh_battery_status.set_level(current_level);

    //? Activar la alarma de bateria baja
    if (!is_charging && !enable_alarm && current_level <= battery_low_level) {
      enable_alarm = true;
      sh_battery_status.set_alarm(true);
      ESP_LOGI(TAG, "Enable alarm battery low");
    }

    //? Desactivar la alarma de bateria baja
    if (enable_alarm) {
      uint8_t level_disble = (battery_low_level + 5);
      if (is_charging || current_level >= level_disble) {
        enable_alarm = false;
        sh_battery_status.set_alarm(false);
        ESP_LOGI(TAG, "Disable alarm battery low");
      }
    }

    ESP_LOGV(TAG, "(Info Battery) Level: %d Low battery: %s Charging %s",
      current_level,
      (enable_alarm) ? "True" : "False",
      (is_charging) ? "True" : "False"
    );
    tick_millis = millis();
  }
}

void show_alarms() {
  static bool last_alarm = false;

  static const uint32_t tick_time = 1000u;
  static ulong tick_count = 0;
  static bool blink_alarm = false;

  if (enable_alarm & !last_alarm) {
    tick_count = millis();
    blink_alarm = false;
  }

  if (enable_alarm) {
    //? Tick alarms
    bool rtem_alarm = sh_temperarute_status.get_alarm();
    if (!rtem_alarm) {
      if (millis() - tick_count >= tick_time) {
        blink_alarm = !blink_alarm;

        if (blink_alarm) {
          led_battery->switch_color(255, 255, 0);
        }
        else {
          led_battery->off();
        }
        tick_count = millis();
      }
    }
  }
  else if (!enable_alarm && last_alarm) {
    //? Deshabilitar alarmas
    led_battery->off();
    blink_alarm = false;
  }
  last_alarm = enable_alarm;
}
