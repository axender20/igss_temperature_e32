#include "adc_battery.h"
#define ANALOG_RESOLUTION 12
void ADCBattery::begin(uint8_t pin, uint16_t min_voltaje, uint16_t max_voltaje) {
  _pin = pin;
  _min = min_voltaje;
  _max = max_voltaje;

  analogReadResolution(ANALOG_RESOLUTION);
  analogSetAttenuation(ADC_11db);
  ema = analogRead(_pin);
}

void ADCBattery::update_status() {
  int raw = analogRead(_pin);
  ema = alpha * raw + (1.0 - alpha) * ema;

  // Convertir a voltaje en mV (3.3V referencia)
  _voltaje = (uint16_t)((ema / 4095.0) * 3300.0);
  
  _level = sigmoidal(_voltaje, _min, _max);
}
