#include <EEPROM.h>
#include <Arduino.h>
#include <FastLED.h>
#include "manager.h"


Error Manager::init() {
  pinMode(LED, OUTPUT);
  pinMode(SPEAKER, OUTPUT);

  Manager::devices[DEVICE_ID].is_active = 1;
  Manager::devices[DEVICE_ID].id = DEVICE_ID;

  Manager::gps.init(GPS_RX, GPS_TX);

  if (!Manager::imu.init(SDA, SCL)) {
    Manager::init_success = 10;
    return FAILED_INIT_IMU;
  }

  if (!Manager::lora.init(LORA1_NSS, LORA1_RST, LORA1_DIO0, LORA1_SCK, LORA1_MISO, LORA1_MOSI)) {
    Manager::init_success = 5;
    return FAILED_INIT_LORA;
  }

  // if (!Manager::elec.init()) {
  //   return FAILED_INIT_ELEC;
  // }

  EEPROM.begin(Manager::imu.storage_length);
  Manager::readFromFlash();

  return SUCCESS;
}

int led_mode = 0;
void Manager::loop() {
  EVERY_N_SECONDS(Manager::init_success) { led_mode = !led_mode; }
  digitalWrite(LED, led_mode);

  EVERY_N_SECONDS(4) { tone(SPEAKER, 440, 2000); }

  Manager::gps.update();

  EVERY_N_MILLISECONDS(UPDATE_INTERVAL) {
    Manager::updateData();
    Manager::transmitData();
  }

  if (Manager::receiveData()) {
    Manager::transmitData();
  }

  // Handle error in some way?
  EVERY_N_MINUTES(WRITE_TO_DISC_INTERVAL) { Manager::writeToFlash(); }

  EVERY_N_SECONDS(1) { Manager::debug(); }
}

void Manager::debug() {
  Serial.printf("Debug\n");
  uint32_t satelites = 0;
  if (!Manager::gps.getSatelites(&satelites)) {
    return;
  }
  Serial.printf("Satelites: %d\n", satelites);

  double alt = 0;
  if (!Manager::gps.getAltitude(&alt)) {
    return;
  }

  Location loc = {0};
  if (!Manager::gps.getLocation(&loc)) {
    return;
  }
  Serial.printf("Alt: %lf, Lat: %lf, lon: %lf\n", loc.lat, loc.lon);

  double northHeading;
  if (!Manager::imu.getNorthHeading(loc.lat, loc.lon, alt, &northHeading)) {
    return;
  }
  Serial.printf("North Heading: %lf\n", northHeading);
}

bool Manager::readFromFlash() {
  uint8_t buf[Manager::imu.storage_length] = { 0 };
  if (EEPROM.readBytes(0, buf, Manager::imu.storage_length) !=
    Manager::imu.storage_length) {
    return false;
  }

  Manager::imu.setOffsetsData(buf);
  return true;
}

bool Manager::writeToFlash() {
  Serial.printf("Writing to disk");

  uint8_t buf[Manager::imu.storage_length] = { 0 };
  if (!Manager::imu.getOffestsData(buf)) {
    return false;
  }

  if (EEPROM.writeBytes(0, buf, Manager::imu.storage_length)
    != Manager::imu.storage_length) {
      return false;
  }

  return EEPROM.commit();
}

bool Manager::receiveData() {
  size_t len = Manager::lora.read((byte*)Manager::tmp_devices, sizeof(Device)*MAX_DEVICES);
  if (len == 0) {
    return false;
  }

  if (len % sizeof(Device) != 0) {
    return false;
  }

  uint8_t amount_of_devices = len / sizeof(Device);
  if (amount_of_devices > MAX_DEVICES) {
    return false;
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

  return is_updated;
}

void Manager::updateData() {
  Location tmpLocation = { 0 };
  if (!Manager::gps.getLocation(&tmpLocation)) {
    return;
  }
  Manager::devices[DEVICE_ID].location = tmpLocation;

  double time;
  if (!Manager::gps.getTime(&time)) {
    return;
  }
  Manager::devices[DEVICE_ID].last_updated = time;

  Manager::last_updated = millis();
}

void Manager::transmitData() {
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
