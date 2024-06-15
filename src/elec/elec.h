// https://learn.adafruit.com/build-a-cloud-connected-esp8266-power-meter/hardware-configuration
// https://electronicstree.com/ina219-current-sensor-with-arduino/#:~:text=INA219%20Module&text=To%20measure%20current%2C%20the%20module,the%20voltage%20across%20the%20load.
// https://cdwilson.dev/articles/understanding-the-ina219/

#include <Wire.h>
#include <Adafruit_INA219.h>

class Elec {
 public:
  Elec();
  bool init(int sda, int scl);
  float getCurrent_mA();
  float getVoltage_V();

 private:
  Adafruit_INA219 _ina;
  TwoWire _wire;
};

