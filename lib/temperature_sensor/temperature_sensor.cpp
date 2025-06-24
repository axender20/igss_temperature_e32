#include "temperature_sensor.h"
#include "Wire.h"
#include "Adafruit_AHTX0.h"
#include "igg_e32_board_pinout.h"

//>Implementacion especifica para el sensor AHTxx y la libreria de Adafruit
//TODO: Cambiar por implementacion real 

Adafruit_AHTX0 aht;
bool _en = true;
void temperature_sensor::init() {
  //> Utiliza Wire para este sensor 
  uint8_t aht_sda = s_temp_sda;
  uint8_t aht_scl = s_temp_scl;

  Wire.setPins(aht_sda, aht_scl);
  bool s_init_aht = aht.begin(&Wire);

  if (!s_init_aht) {
    //TODO: Verificacion de fallo del sensor, realizar acciones
    _en = false;
  }
}

bool temperature_sensor::get_data(float& _temp, float& _value) {
  if (!_en) return false;

  //! Caution: La libreria de adafruit utiliza delay(10 - 20)
  sensors_event_t aht_temp, aht_hum;
  bool read_value = aht.getEvent(&aht_hum, &aht_temp);
  if (read_value) {
    _temp = aht_temp.temperature;
    _value = aht_hum.relative_humidity;
  }
  return read_value;
}