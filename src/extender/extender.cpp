#include "extender.h"
#include "../logger/logger.h"

// I2C address of the BNO055 IMU, which lives on the same bus and must be
// skipped while scanning for the extender.
#define IMU_I2C_ADDRESS 0x29

bool Extender::init() {
  Wire.begin();

  int address = Extender::findAddress();
  if (address == -1) {
    log(ERROR, "failed to find extender address");
    return false;
  }
  log(DEBUG, "Extender adress %d", address);

  return Extender::pcf.begin(address, &Wire);
}

int Extender::findAddress() {
  for (byte address = 8; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0 && address == IMU_I2C_ADDRESS) {
      log(DEBUG, "extender address scan find IMU");
      continue;
    }


    if (error == 0) {
      return address;
    }
  }

  return -1;
}

void Extender::pinMode(uint8_t pin, uint8_t mode) {
  Extender::pcf.pinMode(pin, mode);
}

void Extender::write(uint8_t pin, bool value) {
  Extender::pcf.digitalWrite(pin, value);
}

bool Extender::read(uint8_t pin) {
  return Extender::pcf.digitalRead(pin);
}