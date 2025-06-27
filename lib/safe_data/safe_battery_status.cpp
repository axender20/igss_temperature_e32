#include "safe_battery_status.h"

void SafeBatteryStatus::set_level(const uint8_t level) {
  semph_take();
  _level = level;
  semph_give();
}

void SafeBatteryStatus::set_charge(const bool n_charge) {
  semph_take();
  _is_charging = n_charge;
  semph_give();
}

void SafeBatteryStatus::set_alarm(const bool alarm_battery_low) {
  semph_take();
  _alarm = alarm_battery_low;
  semph_give();
} 