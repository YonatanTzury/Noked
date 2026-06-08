#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PCF8574.h>

// Wraps the PCF8574 I2C I/O extender. The extender drives the MOSFETs that
// power-gate the on-board devices, so it must be initialised before any of
// them. It also owns the single Wire (I2C) bus shared with the IMU.
class Extender
{
public:
  // Brings up the I2C bus, locates the PCF8574 on the bus and begins it.
  // Returns false if no extender is found.
  bool init();

  void pinMode(uint8_t pin, uint8_t mode);
  void write(uint8_t pin, bool value);

private:
  // Scans the I2C bus for the extender, skipping the BNO055 IMU (0x29).
  // Returns the address, or -1 if none found.
  int findAddress();

  Adafruit_PCF8574 pcf;
};
