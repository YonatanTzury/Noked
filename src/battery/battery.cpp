#include "battery.h"

void Battery::init(int pin) {
  Battery::_pin = pin;
  pinMode(pin, INPUT);
  analogSetPinAttenuation(pin, ADC_11db);
}

float Battery::readVoltage() {
  // x2 to undo the 1:2 resistor divider on the sense line.
  return (analogReadMilliVolts(Battery::_pin) / 1000.0f) * 2;
}
