#include <Arduino.h>
#include <utility/imumaths.h>

#include "../logger/logger.h"
#include "imu.h"
#include "XYZgeomag.hpp"

IMU::IMU()
    : bno(Adafruit_BNO055(55, 0x29)) {
}

bool IMU::init() {
  return IMU::bno.begin();
}

bool IMU::getNorthHeading(double lat, double lon, double alt, double* out) {
  geomag::Vector position = geomag::geodetic2ecef(lat, lon, alt);
  geomag::Vector magField = geomag::GeoMag(2022.5, position, geomag::WMM2020);
  geomag::Elements magneticFieldHeadingDiff = geomag::magField2Elements(magField, lat, lon);

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  double heading = euler.x();

  log(DEBUG, "Raw Orientation Data - Heading: %f, Roll: %f, Pitch: %f, Declination: %f",
      heading, euler.y(), euler.z(), magneticFieldHeadingDiff.declination);

  *out = heading + (double)magneticFieldHeadingDiff.declination;

  return true;
}
