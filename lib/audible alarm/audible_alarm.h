#pragma once
#ifndef _AUDIBLE_ALARM_H_
#define _AUDIBLE_ALARM_H_

#include "stdlib.h"

namespace audible_alarm {
  /// @brief Inicializa todo lo que tenga que ver con la alarma audible
  void init();


  /// @brief Activa la alarma audible 
  /// @return (true - success) (false - fail)
  bool on();

  /// @brief Desactiva la alarma audible 
  /// @return (true - success) (false - fail)
  bool off();
};

#endif