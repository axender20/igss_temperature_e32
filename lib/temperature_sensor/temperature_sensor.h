#pragma once
#ifndef _TEMPERATURE_SENSOR_H_
#define _TEMPERATURE_SENSOR_H_

#include "stdlib.h"

namespace temperature_sensor {
  /// @brief Inicializa todo lo que tenga que ver con el sensor
  void init();

  /// @brief Obtiene valores leidos por el sensor xxxx
  /// @param _temp (ref) Valor de temperatura
  /// @param _value (ref) Valor secundario (humedad)
  /// @return bool (true - success) (false - fail)
  bool get_data(float& _temp, float& _value);
};

#endif