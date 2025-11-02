#include <Arduino.h>
#include <utility/imumaths.h>

#include "imu.h"
#include "XYZgeomag.hpp"

IMU::IMU() : _wire(TwoWire(1)), bno(Adafruit_BNO055(55, 0x29)) {}

bool IMU::init(int sda, int scl)
{
  // if (!IMU::_wire.setPins(sda, scl))
  // {
  //   return false;
  // }

  return IMU::bno.begin();
}

bool IMU::getNorthHeading(double lat, double lon, double alt, double *out)
{
  geomag::Vector position = geomag::geodetic2ecef(lat, lon, alt);
  geomag::Vector magField = geomag::GeoMag(2022.5, position, geomag::WMM2020);
  geomag::Elements magneticFieldHeadingDiff = geomag::magField2Elements(magField, lat, lon);

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  double heading = euler.x();
  Serial.printf("Raw Orientation Data - Heading: %f, Roll: %f, Pitch: %f, Declination: %f\n",
                heading, euler.y(), euler.z(), magneticFieldHeadingDiff.declination);

  // Serial.printf("Orientation Data - Heading: %f, Mag Declination: %f\n",
  //               orientationData.orientation.heading,
  //               magneticFieldHeadingDiff.declination);
  *out = heading + (double)magneticFieldHeadingDiff.declination;

  return true;
}
