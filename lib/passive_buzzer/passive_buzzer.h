#pragma once
#ifndef _PASSIVE_BUZZER_H_
#define _PASSIVE_BUZZER_H_

#include <Arduino.h>

class PassiveBuzzer {
public:
    PassiveBuzzer(uint8_t pin, unsigned int freq = 1750);

    void begin();
    void on();                             // Sonar continuo
    void off();                            // Apagar
    void beep(unsigned long duration_ms);  // Sonar por tiempo
    void setFrequency(unsigned int freq);  // Cambiar frecuencia

    void setVolume(uint8_t level);         // Volumen de 0 a 255
    void setVolumePercent(uint8_t percent); // Alternativa: volumen 0 a 100

private:
    uint8_t _pin;
    unsigned int _frequency;
    uint8_t _volume; // 0 - 255
};

#endif
