#include "safe_temp_status.h"

float SafeTemperatureStatus::get_average(bool reset) {
  semph_take();
  float average = (_avg_count > 0) ? _avrg_temp / _avg_count : 0.0f;
  if (reset) {
    _avrg_temp = 0.0f;
    _avg_count = 0u;
  }
  semph_give();
  return average;
}

void SafeTemperatureStatus::add_average(float average) {
  semph_take();
  _avrg_temp += average;
  _avg_count++;
  semph_give();
}

void SafeTemperatureStatus::set_raw(float raw) {
  semph_take();
  _raw_temp = raw;
  semph_give();
}

void SafeTemperatureStatus::set_alarm(bool alarm) {
  semph_take();
  _alarm = alarm;
  semph_give();
}

