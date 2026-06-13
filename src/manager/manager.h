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
  uint32_t last_updated; // GPS Unix epoch seconds of this device's last fix.
};

#define UPDATE_INTERVAL (60 * 1000)        // Minutes
#define DEVICE_ALIVE_TIMEOUT_SEC (5 * 60)  // 5 Minutes, in epoch seconds

#define MILLIS_BUTTON_PRESS 1000
#define DEVICE_USER_FACING_TIMEOUT (1000 * 60 * 2) // 2 minutes

#define MAX_DEVICES 30

// Distance (meters) at or beyond which another device shows at minimum LED
// strength; closer devices ramp linearly up to full strength.
#define LED_MAX_RANGE_METERS 1000.0

// LoRa caps the payload at ~255 bytes; this is how many Devices fit in one packet.
#define MAX_DEVICES_PER_PACKET (255 / sizeof(Device))

// Identifier this unit reports on the LoRa network. Override per-unit at build
// time with a flag, e.g. -DDEVICE_ID=3.
#ifndef DEVICE_ID
#define DEVICE_ID 0
#endif

// When 1, the unit switches between IDLE / USER_FACING / OTHER_DEVICE_USER_FACING
// based on the button and LoRa traffic. When 0 (-DENABLE_MODES=0), it stays in
// USER_FACING permanently.
#ifndef ENABLE_MODES
#define ENABLE_MODES 1
#endif

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
  void drawDevices();
  void transmitData();
  void sendDevices(uint8_t count);
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
