#include <Arduino.h>

// Reads the battery voltage from an ADC pin through a 1:2 divider.
//
// Measured current draw notes (from bring-up):
//   esp + lora                 = 25ma
//   esp + lora + IMU           = 35ma
//   esp + lora + IMU + GPS     = 80-120ma
//   GPS  ~45ma   IMU ~10ma
// An INA219 backend can be added later for current sensing; for now this
// only reports voltage via the on-board divider.
class Battery {
public:
  void init(int pin);
  float readVoltage();

private:
  int _pin = -1;
};
