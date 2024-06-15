#include "temperature.h"

void Temperature::init(int pin) {
  Temperature::_ds = DS18B20(pin);
  Temperature::_ds.selectNext();
}

float Temperature::getTemp() {
  return Temperature::_ds.getTempC();
}
