#include <Arduino.h>
#include "manager.h"

#define lora1_rst 32
#define lora1_nss 15
#define lora1_dio0 26

void Manager::init() {
  SPIClass _hspi = SPIClass(HSPI);
  Manager::lora.init(lora1_nss, lora1_rst, lora1_dio0, _hspi);

  Manager::devices[DEVICE_ID].is_active = 1;
  Manager::update();
}


void Manager::loop() {
  if (Manager::last_updated + UPDATE_INTERVAL < millis()) {
    Manager::update();
  }

  size_t len = Manager::lora.read((byte*)Manager::tmp_devices, sizeof(Device)*MAX_DEVICES);
  if (len == 0) {
    return;
  }

  bool is_updated = false;
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (i == DEVICE_ID) {
      continue;
    }

    if (Manager::devices[i].last_updated > Manager::tmp_devices[i].last_updated) {
      continue;
    }

    // TODO make sure this actully copy the data and not just the pointer
    Manager::devices[i] = Manager::tmp_devices[i];
    is_updated = true;
  }

  if (is_updated) {
    Manager::sync();
  }
}

void Manager::update() {
  Manager::last_updated = millis();

  // TODO change to real time
  Manager::devices[DEVICE_ID].last_updated = millis();
  // TODO read GPS
  Manager::devices[DEVICE_ID].location.lat = 10;
  Manager::devices[DEVICE_ID].location.lon = 10;

  Manager::sync();
}

void Manager::sync() {
  Manager::lora.send((byte*)Manager::devices, sizeof(Device)*MAX_DEVICES);
}
