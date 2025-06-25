#include "audible_alarm.h"
#include "igg_e32_board_pinout.h"
#include "passive_buzzer.h"

PassiveBuzzer buzzer(s_buzzer_pin);

void audible_alarm::init() {
  // ** ledc: 15 => Group: 1, Channel: 7, Timer: 3

  ledcSetup(7, 1750, 8);
  ledcAttachPin(s_buzzer_pin, 7);

  buzzer.begin();
  buzzer.setVolume(255);
}

bool audible_alarm::on() {
  buzzer.on();
  return true;
}

bool audible_alarm::off() {
  buzzer.off();
  return true;
}