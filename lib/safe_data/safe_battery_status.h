#pragma once
#ifndef _SAFE_BATTERY_STATUS_H_
#define _SAFE_BATTERY_STATUS_H_

#include "safe_base.h"

class SafeBatteryStatus : public SafeBase {
private:
  uint8_t _level;
  bool _is_charging;
  bool _alarm;

public:
  SafeBatteryStatus() = default;
  ~SafeBatteryStatus() = default;

  bool init() override {
    _level = 0u;
    _is_charging = false;
    _alarm = false;
    return SafeBase::init();
  }

  void set_level(const uint8_t level);
  void set_charge(const bool n_charge);
  void set_alarm(const bool alarm_battery_low);

  uint8_t get_level() const { return _level; }
  bool is_charging() const { return _is_charging; }
  bool get_alarm() const { return _alarm; }
};

#endif