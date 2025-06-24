#pragma once
#ifndef _SOFTWARE_RTC_H_
#define _SOFTWAER_RTC_H_

#include "date_time.h"

class SoftwareRTC {
private:
  unsigned long ms_cont;
  DateTime c_now;
public:
  SoftwareRTC();
  void setDateTime(DateTime _date_time);
  DateTime now();
};

extern SoftwareRTC s_rtc;
#endif
