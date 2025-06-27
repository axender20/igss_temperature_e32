#pragma once
#ifndef _BATTERY_MONITOR_H_
#define _BATTERY_MONITOR_H_

#include "stdint.h"

namespace battery_monitor {
  /// @brief Inicializa todo lo necesario para la bateria
  void init();

  /// @brief Obtiene el valor del nivel de la bateria
  /// @return (-1 error) (0 - 100) battery level
  int get_blevel();

  /// @brief Obtiene el valor de 
  /// @return (true - charging) (false - non charging) 
  bool is_charging();

  //> Deprecated (Eliminar esta funcion para implementacion real)
  void update();
};

#endif