#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
class IMU {
 public:
  IMU();
  bool init(int sda, int scl);
  bool getNorth(double lat, double lon, double alt, double* out);

 private:
  TwoWire _wire;
  Adafruit_BNO055 bno; 
};
