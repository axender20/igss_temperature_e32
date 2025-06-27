#pragma once
#ifndef _SAFE_BASE_H_
#define _SAFE_BASE_H_

#include "Arduino.h"
#include "semphr.h"

class SafeBase {
protected:
  SemaphoreHandle_t _sempx;

  void semph_take() {
    if (_sempx != NULL) {
      xSemaphoreTake(_sempx, portMAX_DELAY);
    }
  }

  void semph_give() {
    if (_sempx != NULL) {
      xSemaphoreGive(_sempx);
    }
  }

public:
  virtual ~SafeBase() {
    if (_sempx) {
      vSemaphoreDelete(_sempx);
    }
  }

  virtual bool init() {
    _sempx = xSemaphoreCreateBinary();
    if (_sempx == NULL) {
      return false;
    }
    xSemaphoreGive(_sempx);
    return true;
  }
};

#endif