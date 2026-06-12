#include <Arduino.h>

#include "../pins.h"
#include "../lora/lora.h"
#include "../gps/gps.h"
#include "../imu/imu.h"
#include "../extender/extender.h"
#include "../leds/leds.h"
#include "../battery/battery.h"

struct Device {
  bool is_active;
  uint8_t id;
  Location location;
  double last_updated;
};

#define UPDATE_INTERVAL (60 * 1000)      // Minutes
#define DEVICE_ALIVE_TIMOUT (300 * 1000) // 5 Minutes

#define MILLIS_BUTTON_PRESS 1000
#define DEVICE_USER_FACING_TIMEOUT (1000 * 60 * 2) // 2 minutes

#define MAX_DEVICES 30
#define DEVICE_ID 0

enum Error {
  SUCCESS,
  FAILED_INIT_EXTENDER,
  FAILED_INIT_LORA,
};

enum Mode {
  IDLE,
  USER_FACING,
  OTHER_DEVICE_USER_FACING,
};

class Manager {
public:
  Error init();
  void loop();

private:
  void controlGPSPower(bool isOn);
  void controlIMUPower(bool isOn);
  bool initLora();
  void updateGPS();
  void readIMU();
  void transmitData();
  bool receiveData();
  void debug();
  void readLoraAndUpdateMode();

  Device devices[MAX_DEVICES] = { 0 };
  Device tmp_devices[MAX_DEVICES] = { 0 };
  uint8_t id = DEVICE_ID;
  Extender extender;
  Lora lora;
  GPS gps;
  IMU imu;
  Leds leds;
  Battery battery;
  double last_updated = 0;
  Mode mode = IDLE;
  // Latest button state, refreshed from the extender only when its INT line
  // signals a change (see EXT_INT). Avoids polling the button over I2C.
  bool buttonPressed = false;
  // Assume powered on so the first control call drives the gate to a
  // known off state.
  bool gpsPowered = true;
  bool imuPowered = true;
  double heading = 0;
};
