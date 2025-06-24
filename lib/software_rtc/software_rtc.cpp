#include "software_rtc.h"
SoftwareRTC s_rtc;

SoftwareRTC::SoftwareRTC() {
  c_now = DateTime();
  ms_cont = millis();
}

void SoftwareRTC::setDateTime(DateTime _date_time) {
  c_now = _date_time;
  ms_cont = millis();
}

DateTime SoftwareRTC::now() {
  ulong lapse = millis() - ms_cont;  // Esto funciona correctamente aunque millis() se desborde
  DateTime now_time = c_now + TimeSpan(lapse / 1000);
  return now_time;
}