#include <Arduino.h>
#include "manager.h"


Error Manager::init() {
  // SPIClass _hspi = SPIClass(HSPI);
  SPIClass _hspi = SPIClass(VSPI);
  // if (!Manager::lora.init(LORA1_NSS, LORA1_RST, LORA1_DIO0, _hspi)) {
  if (!Manager::lora.init(5, 22, 21, _hspi)) {
    return FAILED_INIT_LORA;
  }

  if (!Manager::imu.init(IMU_SDA, IMU_SCL)) {
    return FAILED_INIT_IMU;
  }

  if (!Manager::elec.init(ELEC_SDA, ELEC_SCL)) {
    return FAILED_INIT_ELEC;
  }

  Manager::gps.init(GPS_RX, GPS_TX);

  Manager::devices[DEVICE_ID].is_active = 1;
  Manager::devices[DEVICE_ID].id = DEVICE_ID;

  return SUCCESS;
}

void Manager::loop() {
  Manager::gps.update();

  if (millis() - Manager::last_updated > UPDATE_INTERVAL) {
    Manager::updateGPS();
  }

  if (Manager::receiveData()) {
    Manager::transmitData();
  }

  Manager::debug();
}

void Manager::debug() {
  Serial.printf("Cur (mA): %f, Vol (V): %f\n",
    Manager::elec.getCurrent_mA(), Manager::elec.getVoltage_V());

  Serial.printf("Temp (c): %f\n", Manager::temperature.getTemp());

  double alt;
  if (!Manager::gps.getAltitude(&alt)) {
    return;
  }

  Location loc;
  if (!Manager::gps.getLocation(&loc)) {
    return;
  }
  Serial.printf("Alt: %d, Lat: %d, lon: %d\n", loc.lat, loc.lon);

  double northHeading;
  if (!Manager::imu.getNorthHeading(loc.lat, loc.lon, alt, &northHeading)) {
    return;
  }

  Serial.printf("North Heading: %d\n", northHeading);
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

void Manager::updateGPS() {
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

  Manager::transmitData();
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
