#include "../imu_gy85/IMUGY85.h"
class IMU {
 public:
  void init();
  void update();
  double getNorth(double lat, double lon, double alt);

 private:
  IMUGY85 imu;
};
