#include "elec.h"

Elec::Elec() : _wire(TwoWire(1)) {}

bool Elec::init(int sda, int scl) {
  if (!Elec::_wire.setPins(sda, scl)) {
    Serial.printf("Failed set pins\n");
    return false;
  }

  Elec::_wire.begin();
  if (!Elec::_ina.begin(&(Elec::_wire))) {
    Serial.println("Could not connect. Fix and Reboot");
    return false;
  }

  Elec::_ina.setCalibration_16V_400mA();

  return true;
}

float Elec::getCurrent_mA() {
  return Elec::_ina.getCurrent_mA();
}

float Elec::getVoltage_V() {
  return Elec::_ina.getBusVoltage_V() + (Elec::_ina.getShuntVoltage_mV() / 1000);
}
