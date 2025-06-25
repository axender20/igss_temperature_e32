#pragma once
#ifndef _SD_LOG_H_
#define _SD_LOG_H_

#include "Arduino.h"
#include "software_rtc.h"
#include "SD.h"
#include "ArduinoLog.h"

extern const char* log_file_dir;

enum e_sdlogger_t : uint8_t {
  e_sdl_error,
  e_sdl_run
};

class SDLogger {
private:
  //? To get the timestamp
  SoftwareRTC _rtc;
  SDFS _sd;
  Logging _log;
  File _log_file;
  bool log_ready();
  bool _status = false; // true: working, false: error
  bool _en_timespan = false; // true: enable, false: disable
  bool _en = true;
  const char* rtc_get_timespan();

  // Helper que abre–redirige–escribe–cierra
  template<typename F>
  void _write_toSD(F writeFn) {
    if (!log_ready()) return;

    File f = _sd.open(log_file_dir, FILE_APPEND, true);
    if (!f) {
      //? Error al abrir el archivo   
      return;
    }

    // Si timespan habilitado, mostrarlo antes
    if (_en_timespan) {
      f.print(rtc_get_timespan());
    }

    _log.begin(LOG_LEVEL_VERBOSE, &f);
    writeFn();
    f.flush();
    f.close();
  }
public:
  SDLogger();

  void set_pinout(SPIClass* _spi, int8_t _sck, int8_t _miso, int8_t _mosi, int8_t _cs);
  void set_enable(bool enable = true) { this->_en = enable; }
  void enable_timespan(DateTime date_time_start);

  /// @brief Obtiene el estado del logger (En funcionamiento - error).
  /// @return bool (status)
  bool get_status() const { return _status; }

  template <class T, typename... Args> void fatal(T msg, Args... args) { _write_toSD([&] { _log.fatal(msg, args...); }); }
  template <class T, typename... Args> void fatalln(T msg, Args... args) { _write_toSD([&] { _log.fatalln(msg, args...); }); }

  template <class T, typename... Args> void error(T msg, Args... args) { _write_toSD([&] { _log.error(msg, args...); }); }
  template <class T, typename... Args> void errorln(T msg, Args... args) { _write_toSD([&] { _log.errorln(msg, args...); }); }

  template <class T, typename... Args> void warning(T msg, Args... args) { _write_toSD([&] { _log.warning(msg, args...); }); }
  template <class T, typename... Args> void warningln(T msg, Args... args) { _write_toSD([&] { _log.warningln(msg, args...); }); }

  template <class T, typename... Args> void notice(T msg, Args... args) { _write_toSD([&] { _log.notice(msg, args...); }); }
  template <class T, typename... Args> void noticeln(T msg, Args... args) { _write_toSD([&] { _log.noticeln(msg, args...); }); }

  template <class T, typename... Args> void info(T msg, Args... args) { _write_toSD([&] { _log.info(msg, args...); }); }
  template <class T, typename... Args> void infoln(T msg, Args... args) { _write_toSD([&] { _log.infoln(msg, args...); }); }

  template <class T, typename... Args> void trace(T msg, Args... args) { _write_toSD([&] { _log.trace(msg, args...); }); }
  template <class T, typename... Args> void traceln(T msg, Args... args) { _write_toSD([&] { _log.traceln(msg, args...); }); }

  template <class T, typename... Args> void verbose(T msg, Args... args) { _write_toSD([&] { _log.verbose(msg, args...); }); }
  template <class T, typename... Args> void verboseln(T msg, Args... args) { _write_toSD([&] { _log.verboseln(msg, args...); }); }

};

#endif