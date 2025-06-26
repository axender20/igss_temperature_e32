#pragma once
#ifndef _SAFE_TEMPERATURE_STATUS_H_
#define _SAFE_TEMPERATURE_STATUS_H_

#include "Arduino.h"
#include <freertos/semphr.h>

class SafeTemperatureStatus {
private:
  SemaphoreHandle_t _sempx;
  float _raw_temp;
  bool _alarm;

  /*Basado en un promedio: (Rolling Average) */
  float _avrg_temp;
  uint16_t _avg_count;

  void semph_take();
  void semph_give();
public:
  SafeTemperatureStatus() {};
  ~SafeTemperatureStatus() {};

  bool init();

  float get_average(bool reset = true);
  float get_raw()const { return _raw_temp; }
  bool get_alarm()const { return _alarm; }

  void add_average(float average);
  void set_raw(float raw);
  void set_alarm(bool alarm);
};

#endif