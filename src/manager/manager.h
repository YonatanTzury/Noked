#include <Arduino.h>

#include "..\lora\lora.h"
#include "..\gps\gps.h"
#include "..\imu\imu.h"
#include "..\temperature\temperature.h"
#include "..\elec\elec.h"

struct Device
{
  bool is_active;
  uint8_t id;
  Location location;
  unsigned long last_updated;
};

#define UPDATE_INTERVAL (60 * 1000) // Minutes
#define DEVICE_ALIVE_TIMOUT (300 * 1000) // 5 Minutes
#define WRITE_TO_DISC_INTERVAL 10 // Minutes

#define MAX_DEVICES 30
#define DEVICE_ID 0

// PINS
#define TEMPERATURE_PIN 18

#define LORA1_RST 32
#define LORA1_NSS 15
#define LORA1_DIO0 26

#define GPS_RX 16
#define GPS_TX 17

enum Error {
  SUCCESS,
  FAILED_INIT_LORA,
  FAILED_INIT_IMU,
  FAILED_INIT_ELEC
};
class Manager {
 public:
  Error init();
  void loop();

 private:
  void updateData();
  void transmitData();
  bool receiveData();
  void debug();
  bool writeToFlash();
  bool readFromFlash();

  Device devices[MAX_DEVICES] = { 0 };
  Device tmp_devices[MAX_DEVICES] = { 0 };
  uint8_t id = DEVICE_ID;
  Lora lora = Lora(HSPI);
  GPS gps;
  IMU imu;
  Temperature temperature = Temperature(TEMPERATURE_PIN);
  Elec elec;

  unsigned long last_updated = 0;
};
