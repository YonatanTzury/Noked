#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
class IMU {
 public:
  IMU();
  bool init();
  double getNorth(double lat, double lon, double alt);

 private:
  TwoWire _wire;
  Adafruit_BNO055 bno; 
};
