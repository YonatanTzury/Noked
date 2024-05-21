#include <Arduino.h>

#include "..\lora\lora.h"

struct Location
{
  unsigned long lat;
  unsigned long lon;
};

struct Device
{
  bool is_active;
  Location location;
  unsigned long last_updated;
};

#define UPDATE_INTERVAL (60 * 1000) // MINUTE

#define MAX_DEVICES 30
#define DEVICE_ID 0

class Manager {
 public:
  void init();
  void loop();

 private:
  void update();
  void sync();

  Device devices[MAX_DEVICES] = { 0 };
  Device tmp_devices[MAX_DEVICES] = { 0 };
  uint8_t id = DEVICE_ID;
  Lora lora;
  unsigned long last_updated;
};
