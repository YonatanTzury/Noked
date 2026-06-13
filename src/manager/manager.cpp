#include <Arduino.h>
#include "../logger/logger.h"
#include "manager.h"

void IRAM_ATTR onExtenderInterrupt();

Error Manager::init() {
  Manager::devices[DEVICE_ID].is_active = 1;
  Manager::devices[DEVICE_ID].id = DEVICE_ID;

  if (!Manager::extender.init()) {
    log(ERROR, "Manager init failed: extender init failed");
    return FAILED_INIT_EXTENDER;
  }

  Manager::extender.pinMode(EXT_BUTTON, INPUT);
  Manager::extender.pinMode(EXT_GPS_POWER, OUTPUT);
  Manager::extender.pinMode(EXT_IMU_POWER, OUTPUT);

  // Seed the cached button state and clear any latched extender INT, then
  // react to button changes via the extender's INT line instead of polling.
  Manager::buttonPressed = Manager::extender.read(EXT_BUTTON);
  pinMode(EXT_INT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EXT_INT), onExtenderInterrupt, FALLING);

  // Start with both power gates off in a known state.
  Manager::controlGPSPower(false);
  Manager::controlIMUPower(false);

  if (!Manager::initLora()) {
    log(ERROR, "Manager init failed: LoRa init failed");
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

// Set by the extender's INT line; the loop reads the button over I2C only
// when this is set (I2C is unsafe inside an ISR).
volatile bool extenderInterruptFlag = false;
void IRAM_ATTR onExtenderInterrupt() {
  extenderInterruptFlag = true;
}

void Manager::readLoraAndUpdateMode() {
#if !ENABLE_MODES
  // Mode switching disabled at build time: keep draining incoming LoRa so the
  // neighbor table stays fresh, but never leave USER_FACING.
  Manager::receiveData();
  Manager::mode = USER_FACING;
  return;
#endif

  unsigned long now = millis();
  if (Manager::receiveData()) {
    timeSetOtherDeviceUserFacing = now;
  }

  // Refresh the cached button state only when the extender signalled a change.
  if (extenderInterruptFlag) {
    extenderInterruptFlag = false;
    Manager::buttonPressed = Manager::extender.read(EXT_BUTTON);
  }

  if (!Manager::buttonPressed) {
    lastTimeBottonUnPressed = now;
  }
  unsigned long durationButtonPressed = now - lastTimeBottonUnPressed;
  if (durationButtonPressed >= MILLIS_BUTTON_PRESS) {
    timeSetUserFaceing = now;
  }

  // Subtraction form is rollover-safe; the != 0 guard prevents a cold-start unit
  // (timers default to 0) from latching into USER_FACING for the first timeout.
  if (timeSetUserFaceing != 0 && now - timeSetUserFaceing <= DEVICE_USER_FACING_TIMEOUT) {
    Manager::mode = USER_FACING;
    return;
  }

  if (timeSetOtherDeviceUserFacing != 0 && now - timeSetOtherDeviceUserFacing <= DEVICE_USER_FACING_TIMEOUT) {
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
  log(DEBUG, "Setting gps power %d", on);
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
  log(DEBUG, "Setting imu power %d", on);
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
  log(DEBUG, "Manager mode (0: idle, 1: user, 2: other user) %d", Manager::mode);
  log(DEBUG, "Battery: %f", Manager::battery.readVoltage());
  log(DEBUG, "Satellites: %d", Manager::gps.getSatellites());

  double alt;
  if (!Manager::gps.getAltitude(&alt)) {
    log(DEBUG, "no gps altitude");
    return;
  }

  Location loc;
  if (!Manager::gps.getLocation(&loc)) {
    log(DEBUG, "no gps location");
    return;
  }
  log(DEBUG, "Alt: %f, Lat: %f, lon: %f", alt, loc.lat, loc.lon);

  double northHeading;
  if (!Manager::imu.getNorthHeading(loc.lat, loc.lon, alt, &northHeading)) {
    log(DEBUG, "No IMU north heading");
    return;
  }

  log(DEBUG, "North Heading: %f", northHeading);

  for (int i = 0; i < MAX_DEVICES; i++) {
    if (!Manager::devices[i].is_active) {
      continue;
    }
    Device& d = Manager::devices[i];
    log(DEBUG, "Device id: %u, lat: %f, lon: %f, last_updated: %lu",
        d.id, d.location.lat, d.location.lon, (unsigned long)d.last_updated);
  }
}

bool Manager::receiveData() {
  size_t packetSize = 0;
  // A single LoRa packet holds at most MAX_DEVICES_PER_PACKET Devices, so only
  // offer that much of the buffer; read() rejects anything larger.
  size_t len = Manager::lora.read((byte*)Manager::tmp_devices, sizeof(Device) * MAX_DEVICES_PER_PACKET, &packetSize);
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
    uint8_t id = Manager::tmp_devices[i].id;
    if (id == DEVICE_ID) {
      continue;
    }

    // The id comes off the wire, so bound it before indexing the table.
    if (id >= MAX_DEVICES) {
      continue;
    }

    // Compare against the slot we are about to write, not the packet position
    // (the sender compacts active devices, so position != id).
    if (Manager::devices[id].last_updated >= Manager::tmp_devices[i].last_updated) {
      continue;
    }

    Manager::devices[id] = Manager::tmp_devices[i];
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

  uint32_t time = Manager::gps.getTime();
  Manager::devices[DEVICE_ID].last_updated = time;

  Manager::last_updated = millis();
}

void Manager::transmitData() {
  uint32_t now = Manager::gps.getTime();

  uint8_t counter = 0;
  for (int i = 0; i < MAX_DEVICES; i++) {
    if (Manager::devices[i].is_active == 0) {
      continue;
    }

    // Drop stale neighbors. Skip the filter until we have a GPS epoch (now == 0),
    // so a fresh boot without a fix still forwards what it knows.
    if (now != 0 && now - Manager::devices[i].last_updated > DEVICE_ALIVE_TIMEOUT_SEC) {
      continue;
    }

    Manager::tmp_devices[counter] = Manager::devices[i];
    counter++;

    // A full packet's worth is collected; flush it and keep going so the
    // overflow is carried in further packets instead of being dropped.
    if (counter == MAX_DEVICES_PER_PACKET) {
      Manager::sendDevices(counter);
      counter = 0;
    }
  }

  // Flush the trailing partial packet (also the only packet in the common case).
  if (counter > 0) {
    Manager::sendDevices(counter);
  }
}

void Manager::sendDevices(uint8_t count) {
  if (Manager::lora.send((byte*)Manager::tmp_devices, sizeof(Device) * count) == 0) {
    log(WARN, "lora send failed");
  }
}
