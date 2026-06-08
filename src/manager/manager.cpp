#include <Arduino.h>
#include "manager.h"

Error Manager::init()
{
  Manager::devices[DEVICE_ID].is_active = 1;
  Manager::devices[DEVICE_ID].id = DEVICE_ID;

  // The extender brings up the shared I2C bus and the device power gates,
  // so it must be initialised first.
  if (!Manager::extender.init())
  {
    return FAILED_INIT_EXTENDER;
  }

  // Power on the GPS via its MOSFET gate.
  Manager::extender.pinMode(EXT_GPS_POWER, OUTPUT);
  Manager::extender.write(EXT_GPS_POWER, LOW);

  Manager::leds.init();

  Manager::battery.init(BATTERY_ADC);

  Manager::gps.init(GPS_RX, GPS_TX);

  // Pulse the LoRa reset line (routed through the extender) before init.
  Manager::extender.resetPulse(EXT_LORA_RST);

  if (!Manager::lora.init(LORA_NSS, LORA_RST, LORA_DIO0))
  {
    return FAILED_INIT_LORA;
  }

  if (!Manager::imu.init(IMU_SDA, IMU_SCL))
  {
    return FAILED_INIT_IMU;
  }

  return SUCCESS;
}

void Manager::loop()
{
  Manager::gps.update();

  if (millis() - Manager::last_updated > UPDATE_INTERVAL)
  {
    Manager::updateGPS();
  }

  if (Manager::receiveData())
  {
    Manager::transmitData();
  }

  Manager::debug();
}

void Manager::debug()
{

  double alt;
  if (!Manager::gps.getAltitude(&alt))
  {
    return;
  }

  Location loc;
  if (!Manager::gps.getLocation(&loc))
  {
    return;
  }
  Serial.printf("Alt: %f, Lat: %f, lon: %f\n", alt, loc.lat, loc.lon);

  Serial.printf("Battery: %f\n", Manager::battery.readVoltage());

  double northHeading;
  if (!Manager::imu.getNorthHeading(loc.lat, loc.lon, alt, &northHeading))
  {
    return;
  }

  Serial.printf("North Heading: %f\n", northHeading);
}

bool Manager::receiveData()
{
  size_t packetSize = 0;
  size_t len = Manager::lora.read((byte *)Manager::tmp_devices, sizeof(Device) * MAX_DEVICES, &packetSize);
  if (len == 0)
  {
    return false;
  }

  if (len % sizeof(Device) != 0)
  {
    return false;
  }

  uint8_t amount_of_devices = len / sizeof(Device);
  if (amount_of_devices > MAX_DEVICES)
  {
    return false;
  }

  bool is_updated = false;
  for (int i = 0; i < amount_of_devices; i++)
  {
    if (Manager::tmp_devices[i].id == DEVICE_ID)
    {
      continue;
    }

    if (Manager::devices[i].last_updated > Manager::tmp_devices[i].last_updated)
    {
      continue;
    }

    Manager::devices[Manager::tmp_devices[i].id] = Manager::tmp_devices[i];
    is_updated = true;
  }

  return is_updated;
}

void Manager::updateGPS()
{
  Location tmpLocation = {0};
  if (!Manager::gps.getLocation(&tmpLocation))
  {
    return;
  }
  Manager::devices[DEVICE_ID].location = tmpLocation;

  double time;
  if (!Manager::gps.getTime(&time))
  {
    return;
  }
  Manager::devices[DEVICE_ID].last_updated = time;

  Manager::last_updated = millis();

  Manager::transmitData();
}

void Manager::transmitData()
{
  uint8_t counter = 0;
  for (int i = 0; i < MAX_DEVICES; i++)
  {
    if (Manager::devices[DEVICE_ID].is_active == 0)
    {
      continue;
    }

    if (millis() - Manager::devices[DEVICE_ID].last_updated > DEVICE_ALIVE_TIMOUT)
    {
      continue;
    }

    Manager::tmp_devices[counter] = Manager::devices[i];
    counter++;
  }

  Manager::lora.send((byte *)Manager::tmp_devices, sizeof(Device) * counter);
}
