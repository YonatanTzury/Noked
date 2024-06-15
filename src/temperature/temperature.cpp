#include "temperature.h"

Temperature::Temperature(int pin) : _ds(DS18B20(pin)) {
  Temperature::_ds.selectNext();
}

float Temperature::getTemp() {
  return Temperature::_ds.getTempC();
}
