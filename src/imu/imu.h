#include <Wire.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
class IMU {
public:
  IMU();
  bool init();
  bool getNorthHeading(double lat, double lon, double alt, double* out);

private:
  Adafruit_BNO055 bno;
};
