#include "passive_buzzer.h"

PassiveBuzzer::PassiveBuzzer(uint8_t pin, unsigned int freq)
  : _pin(pin), _frequency(freq), _volume(255) {
}

void PassiveBuzzer::begin() {
  pinMode(_pin, OUTPUT);
  off();
}

void PassiveBuzzer::on() {
  if (_volume == 0) {
    off();
    return;
  }

  if (_volume >= 255) {
    tone(_pin, _frequency);
    return;
  }

  // Simula volumen PWM con ciclos de encendido/apagado
  for (int i = 0; i < 50; i++) {
    tone(_pin, _frequency);
    delayMicroseconds(map(_volume, 0, 255, 100, 1000));  // encendido
    noTone(_pin);
    delayMicroseconds(map(255 - _volume, 0, 255, 100, 1000)); // apagado
  }
}

void PassiveBuzzer::off() {
  noTone(_pin);
}

void PassiveBuzzer::beep(unsigned long duration_ms) {
  tone(_pin, _frequency);
  delay(duration_ms);
  off();
}

void PassiveBuzzer::setFrequency(unsigned int freq) {
  _frequency = freq;
}

void PassiveBuzzer::setVolume(uint8_t level) {
  if (level > 255) level = 255;
  _volume = level;
}

void PassiveBuzzer::setVolumePercent(uint8_t percent) {
  if (percent > 100) percent = 100;
  _volume = map(percent, 0, 100, 0, 255);
}
