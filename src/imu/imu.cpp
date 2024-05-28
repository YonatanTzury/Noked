#include "imu.h"
#include "XYZgeomag.hpp"

void IMU::init()
{
  IMU::imu.init();
}

double IMU::getNorth(double lat, double lon, double alt) {
  geomag::Vector position = geomag::geodetic2ecef(lat, lon, alt);
  geomag::Vector mag_field = geomag::GeoMag(2022.5, position, geomag::WMM2020);
  geomag::Elements out = geomag::magField2Elements(mag_field, lat, lon);

  return imu.getYaw() + out.declination;
}

void IMU::update() {
  IMU::imu.update();
}
