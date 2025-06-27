#pragma once
#ifndef _SAFE_TEMPERATURE_STATUS_H_
#define _SAFE_TEMPERATURE_STATUS_H_

#include "safe_base.h"

class SafeTemperatureStatus : public SafeBase {
private:
  float _raw_temp;
  bool _alarm;

  /*Basado en un promedio: (Rolling Average) */
  float _avrg_temp;
  uint16_t _avg_count;

public:
  SafeTemperatureStatus() = default;
  ~SafeTemperatureStatus() = default;

  bool init() override {
    _alarm = false;
    _raw_temp = 0.0f;
    _avrg_temp = 0.0f;
    _avg_count = 0u;
    return SafeBase::init();
  }

  float get_average(bool reset = true);
  float get_raw()const { return _raw_temp; }
  bool get_alarm()const { return _alarm; }

  void add_average(float average);
  void set_raw(float raw);
  void set_alarm(bool alarm);
};

#endif