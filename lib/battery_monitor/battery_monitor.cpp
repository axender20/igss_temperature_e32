#include "battery_monitor.h"
#include "adc_battery.h"
#include "igg_e32_board_pinout.h"

static ADCBattery batt;
static bool batt_ischg = false;

static bool is_press(uint8_t pin, bool pullup = true, unsigned long debounceTime = 30) {
  static bool lastStableState = false;
  static unsigned long lastChangeTime = 0;

  if (pullup)
    pinMode(pin, INPUT_PULLUP);
  else
    pinMode(pin, INPUT);

  bool reading = digitalRead(pin);

  //> Invertimos si pull-up está activo (LOW == presionado)
  bool currentState = pullup ? !reading : reading;

  if (currentState != lastStableState) {
    if (millis() - lastChangeTime > debounceTime) {
      lastStableState = currentState;
      lastChangeTime = millis();
    }
  }
  else {
    lastChangeTime = millis();  // mantiene referencia fresca
  }

  return lastStableState;
}

//? Valores obtenidos en base a la bateria 500mAH (No Utilizable)
//? [1174256][I][main.cpp:41] thread_1(): [ADC_THREAD] mv: min=1078 max=2576 | raw: min=1287 max=3189

void battery_monitor::init() {

  // _voltaje = (uint16_t)((ema / 4095.0) * 3300.0);

  batt.begin(b_battery_adc, 1036, 2570);
  pinMode(b_battery_charg, INPUT_PULLUP);
}


void battery_monitor::update() {
  //? Actualiza el estado de la bateria
  batt.update_status();
  batt_ischg = is_press(b_battery_charg, true, 500);
}

int battery_monitor::get_blevel() {
  bool validate = true;
  if (!validate) return -1;

  return batt.get_level();
}

bool battery_monitor::is_charging() {
  return batt_ischg;
}

