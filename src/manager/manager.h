#include <Arduino.h>

#include "../pins.h"
#include "../lora/lora.h"
#include "../gps/gps.h"
#include "../imu/imu.h"
#include "../extender/extender.h"
#include "../leds/leds.h"
#include "../battery/battery.h"

struct Device
{
  bool is_active;
  uint8_t id;
  Location location;
  double last_updated;
};

#define UPDATE_INTERVAL (60 * 1000)      // Minutes
#define DEVICE_ALIVE_TIMOUT (300 * 1000) // 5 Minutes

#define MAX_DEVICES 30
#define DEVICE_ID 0

enum Error
{
  SUCCESS,
  FAILED_INIT_EXTENDER,
  FAILED_INIT_LORA,
  FAILED_INIT_IMU,
  FAILED_INIT_ELEC
};
class Manager
{
public:
  Error init();
  void loop();

private:
  void updateGPS();
  void transmitData();
  bool receiveData();
  void debug();

  Device devices[MAX_DEVICES] = {0};
  Device tmp_devices[MAX_DEVICES] = {0};
  uint8_t id = DEVICE_ID;
  Extender extender;
  Lora lora = Lora(FSPI);
  GPS gps;
  IMU imu;
  Leds leds;
  Battery battery;
  double last_updated = 0;
};
