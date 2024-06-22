#include <Wire.h>

#include "elec.h"

bool Elec::init() {
  if (!Elec::_ina.begin(&Wire)) {
    return false;
  }

  Elec::_ina.setCalibration_32V_2A();

  return true;
}

float Elec::getCurrent_mA() {
  return Elec::_ina.getCurrent_mA();
}

float Elec::getVoltage_V() {
  return Elec::_ina.getBusVoltage_V() + (Elec::_ina.getShuntVoltage_mV() / 1000);
}
