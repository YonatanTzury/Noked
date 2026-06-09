#include <Arduino.h>
#include "manager.h"

Error Manager::init() {
  Manager::devices[DEVICE_ID].is_active = 1;
  Manager::devices[DEVICE_ID].id = DEVICE_ID;

  if (!Manager::extender.init()) {
    return FAILED_INIT_EXTENDER;
  }

  Manager::extender.pinMode(EXT_BUTTON, INPUT);
  Manager::extender.pinMode(EXT_GPS_POWER, OUTPUT);
  Manager::extender.pinMode(EXT_IMU_POWER, OUTPUT);

  // Start with both power gates off in a known state.
  Manager::controlGPSPower(false);
  Manager::controlIMUPower(false);

  if (!Manager::initLora()) {
    return FAILED_INIT_LORA;
  }

  Manager::leds.init();
  Manager::battery.init(BATTERY_ADC);

  return SUCCESS;
}

void Manager::loop() {
  Manager::readLoraAndUpdateMode();

  switch (Manager::mode) {
  case IDLE:
    // 1. if gps / IMU is on -> turn off
    Manager::controlGPSPower(false);
    Manager::controlIMUPower(false);
    break;

  case USER_FACING:
    Manager::controlIMUPower(true);

    Manager::controlGPSPower(true);
    Manager::gps.update();

    // TODO: read IMU and show in leds + turn off leds

    EVERY_N_SECONDS(1) {
      Manager::updateGPS();
      Manager::transmitData();
    }
    break;

  case OTHER_DEVICE_USER_FACING:
    Manager::controlIMUPower(false);

    Manager::controlGPSPower(true);
    Manager::gps.update();

    EVERY_N_SECONDS(1) {
      Manager::updateGPS();
      Manager::transmitData();
    }
    break;
  }

  EVERY_N_SECONDS(1) {
    Manager::debug();
  }
}

unsigned long lastTimeBottonUnPressed = 0;
unsigned long timeSetUserFaceing = 0;
unsigned long timeSetOtherDeviceUserFacing = 0;

void Manager::readLoraAndUpdateMode() {
  unsigned long now = millis();
  if (Manager::receiveData()) {
    timeSetOtherDeviceUserFacing = now;
  }

  if (Manager::extender.read(EXT_BUTTON) == 0) {
    lastTimeBottonUnPressed = now;
  }
  unsigned long durationButtonPressed = now - lastTimeBottonUnPressed;
  if (durationButtonPressed >= MILLIS_BUTTON_PRESS) {
    timeSetUserFaceing = now;
  }

  if (now <= timeSetUserFaceing + DEVICE_USER_FACING_TIMEOUT) {
    Manager::mode = USER_FACING;
    return;
  }

  if (now <= timeSetOtherDeviceUserFacing + DEVICE_USER_FACING_TIMEOUT) {
    Manager::mode = OTHER_DEVICE_USER_FACING;
    return;
  }

  Manager::mode = IDLE;
}

void Manager::controlGPSPower(bool on) {
  if (Manager::gpsPowered == on) {
    return;
  }

  if (!on) {
    Manager::gps.stop();
  }

  Manager::gpsPowered = on;
  Manager::extender.write(EXT_GPS_POWER, on ? LOW : HIGH);

  if (on) {
    Manager::gps.init(GPS_RX, GPS_TX);
  }
}

void Manager::controlIMUPower(bool on) {
  if (Manager::imuPowered == on) {
    return;
  }
  Manager::imuPowered = on;
  Manager::extender.write(EXT_IMU_POWER, on ? LOW : HIGH);

  if (on) {
    Manager::imu.init();
  }
}

void Manager::readIMU() {
  double alt;
  Location loc;
  if (!Manager::gps.getAltitude(&alt) || !Manager::gps.getLocation(&loc)) {
    return;
  }

  Manager::imu.getNorthHeading(loc.lat, loc.lon, alt, &this->heading);
}

bool Manager::initLora() {
  // Pulse the LoRa reset line (routed through the extender) before init.
  Manager::extender.pinMode(EXT_LORA_RST, OUTPUT);
  Manager::extender.write(EXT_LORA_RST, LOW);
  delay(10);
  Manager::extender.write(EXT_LORA_RST, HIGH);
  delay(10);

  return Manager::lora.init(LORA_NSS, LORA_RST, LORA_DIO0);
}

void Manager::debug() {

  double alt;
  if (!Manager::gps.getAltitude(&alt)) {
    return;
  }

  Location loc;
  if (!Manager::gps.getLocation(&loc)) {
    return;
  }
  Serial.printf("Alt: %f, Lat: %f, lon: %f\n", alt, loc.lat, loc.lon);

  Serial.printf("Battery: %f\n", Manager::battery.readVoltage());

  double northHeading;
  if (!Manager::imu.getNorthHeading(loc.lat, loc.lon, alt, &northHeading)) {
    return;
  }

  Serial.printf("North Heading: %f\n", northHeading);
}

bool Manager::receiveData() {
  size_t packetSize = 0;
  size_t len = Manager::lora.read((byte*)Manager::tmp_devices, sizeof(Device) * MAX_DEVICES, &packetSize);
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

  double time = Manager::gps.getTime();
  Manager::devices[DEVICE_ID].last_updated = time;

  Manager::last_updated = millis();
}

void Manager::transmitData() {
  uint8_t counter = 0;
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (Manager::devices[i].is_active == 0) {
      continue;
    }

    if (millis() - Manager::devices[i].last_updated > DEVICE_ALIVE_TIMOUT) {
      continue;
    }

    Manager::tmp_devices[counter] = Manager::devices[i];
    counter++;
  }

  Manager::lora.send((byte*)Manager::tmp_devices, sizeof(Device) * counter);
}
