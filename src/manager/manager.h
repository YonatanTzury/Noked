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
  double last_updated;
};

#define UPDATE_INTERVAL (60 * 1000) // Minutes
#define DEVICE_ALIVE_TIMOUT (300 * 1000) // 5 Minutes

#define MAX_DEVICES 30
#define DEVICE_ID 0

class Manager {
 public:
  Manager();
  void init();
  void loop();

 private:
  void update();
  void sync();

  Device devices[MAX_DEVICES] = { 0 };
  Device tmp_devices[MAX_DEVICES] = { 0 };
  uint8_t id = DEVICE_ID;
  Lora lora;
  GPS gps;
  IMU imu;
  Temperature temperature;
  Elec elec;
  double last_updated;
};
