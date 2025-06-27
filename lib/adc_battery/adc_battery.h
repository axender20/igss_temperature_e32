#pragma once
#ifndef _ADC_BATTERY_H_
#define _ADC_BATTERY_H_

#include "Arduino.h"

class ADCBattery {
private:
  uint16_t _min;
  uint16_t _max;

  static inline uint8_t sigmoidal(uint16_t voltage, uint16_t minVoltage, uint16_t maxVoltage) {
    if (voltage <= minVoltage) return 0;
    if (voltage >= maxVoltage) return 100;

    float norm = (float)(voltage - minVoltage) / (maxVoltage - minVoltage);
    float exponent = pow(1.724 * norm, 5.5);
    uint8_t result = 105 - (105 / (1 + exponent));

    return result;
  }

  float ema = 0.0;
  const float alpha = 0.1;
  uint8_t _pin;
  uint8_t _level;
  uint16_t _voltaje;
public:
  ADCBattery(/* args */) { ; }
  ~ADCBattery() { ; }

  void begin(uint8_t pin, uint16_t min_voltaje, uint16_t max_voltaje);

  void update_status();

  uint8_t get_level() const { return _level; }
  uint16_t get_voltaje() const { return _voltaje; }
};

#endif