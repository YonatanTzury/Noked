#include <Adafruit_BNO055.h>
#include <Adafruit_Sensor.h>
class IMU {
 public:
  IMU();
  bool init();
  bool getNorthHeading(double lat, double lon, double alt, double* out);
  bool getOffestsData(uint8_t* buf);
  void setOffsetsData(uint8_t* buf);

  static const int storage_length = sizeof(adafruit_bno055_offsets_t);

 private:
  Adafruit_BNO055 bno; 
};
