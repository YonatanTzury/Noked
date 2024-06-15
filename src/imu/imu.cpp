#include <Arduino.h>
#include <utility/imumaths.h>

#include "imu.h"
#include "XYZgeomag.hpp"

IMU::IMU() : _wire(TwoWire(1)), bno(Adafruit_BNO055(55, 0x29, &_wire)) {}

bool IMU::init(int sda, int scl) {
  if (!IMU::_wire.setPins(sda, scl)) {
    return false;
  }
  
  return IMU::bno.begin();
}

bool IMU::getNorthHeading(double lat, double lon, double alt, double* out) {
  geomag::Vector position = geomag::geodetic2ecef(lat, lon, alt);
  geomag::Vector magField = geomag::GeoMag(2022.5, position, geomag::WMM2020);
  geomag::Elements magneticFieldHeadingDiff = geomag::magField2Elements(magField, lat, lon);

  sensors_event_t orientationData;
  if (!bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER)) {
    return false;
  }

  *out =  orientationData.orientation.heading + magneticFieldHeadingDiff.declination;

  return true;
}
