#include "sd_logger.h"

//? SD Includes
#include "FS.h"
#include "SD.h"
#include "FSImpl.h"
#include "vfs_api.h"
#include "SPI.h"

const char* log_file_dir = "/sd_log.txt";

bool SDLogger::log_ready() {
  //? Si es necesirio agregar validaciones 
  bool result = _status & _en;
  return result;
}

const char* SDLogger::rtc_get_timespan() {
  static char buffer[32];
  DateTime now = _rtc.now();
  snprintf(buffer, sizeof(buffer), "[%04d-%02d-%02d %02d:%02d:%02d] ",
    now.year(), now.month(), now.day(),
    now.hour(), now.minute(), now.second());
  return buffer;
}


SDLogger::SDLogger() :
  _rtc(SoftwareRTC()), _sd(FSImplPtr(new VFSImpl())), _log(Logging()), _log_file(File()) {
  _status = false;
}

void SDLogger::set_pinout(SPIClass* _spi, int8_t _sck, int8_t _miso, int8_t _mosi, int8_t _cs) {
  //? Default values to try it
  const uint8_t max_attempts = 5u;
  const uint8_t retry_delay_ms = 100u;

  if (_spi == NULL) return;

  _spi->setFrequency(4000000);
  _spi->begin(_sck, _miso, _mosi, _cs);

  bool mounted = false;

  for (int attempt = 1; attempt <= max_attempts; ++attempt) {
    mounted = _sd.begin(_cs, *_spi);

    if (mounted) {
      _status = true;
      return;
    }

    if (attempt == max_attempts) break;

    delay(retry_delay_ms);
  }
  _status = false;
}

void SDLogger::enable_timespan(DateTime date_time_start) {
  if (!log_ready()) return;
  _en_timespan = true;
  _rtc.setDateTime(date_time_start);
}

