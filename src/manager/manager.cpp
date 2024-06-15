#include <Arduino.h>
#include "manager.h"

#define lora1_rst 32
#define lora1_nss 15
#define lora1_dio0 26

#define temperature_pin 2

#define elec_scl 12
#define elec_sda 13

Manager::Manager() : temperature(Temperature(temperature_pin)) {}

void Manager::init() {
  SPIClass _hspi = SPIClass(HSPI);
  Manager::lora.init(lora1_nss, lora1_rst, lora1_dio0, _hspi);
  Manager::gps.init();
  Manager::imu.init();
  Manager::elec.init(elec_sda, elec_scl);

  Manager::devices[DEVICE_ID].is_active = 1;
  Manager::devices[DEVICE_ID].id = DEVICE_ID;
  Manager::update();
}


void Manager::loop() {
  Manager::gps.update();

  if (millis() - Manager::last_updated > UPDATE_INTERVAL) {
    Manager::update();
  }

  size_t len = Manager::lora.read((byte*)Manager::tmp_devices, sizeof(Device)*MAX_DEVICES);
  if (len == 0) {
    return;
  }

  if (len % sizeof(Device) != 0) {
    return;
  }

  uint8_t amount_of_devices = len / sizeof(Device);
  if (amount_of_devices > MAX_DEVICES) {
    return;
  }

  bool is_updated = false;
  for (int i = 0; i < amount_of_devices; i++) {
    if (Manager::tmp_devices[i].id == DEVICE_ID) {
      continue;
    }

    if (Manager::devices[i].last_updated > Manager::tmp_devices[i].last_updated) {
      continue;
    }

    Manager::devices[Manager::tmp_devices[i].id] = Manager::tmp_devices[i];
    is_updated = true;
  }

  if (is_updated) {
    Manager::sync();
  }
}

void Manager::update() {
  Location tmpLocation = { 0 };
  if (!Manager::gps.getLocation(&tmpLocation)) {
    return;
  }
  Manager::devices[DEVICE_ID].location = tmpLocation;
  Manager::devices[DEVICE_ID].last_updated = Manager::gps.getTime();

  Manager::last_updated = millis();

  Manager::sync();
}

void Manager::sync() {
  uint8_t counter = 0;
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (Manager::devices[DEVICE_ID].is_active == 0) {
      continue;
    }

    if (millis() - Manager::devices[DEVICE_ID].last_updated > DEVICE_ALIVE_TIMOUT) {
      continue;
    }

    Manager::tmp_devices[counter] = Manager::devices[i];
    counter++;
  }

  Manager::lora.send((byte*)Manager::tmp_devices, sizeof(Device)*counter);
}
