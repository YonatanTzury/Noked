#include <Wire.h>
#include <Arduino.h>
#include <utility/imumaths.h>

#include "imu.h"
#include "XYZgeomag.hpp"

IMU::IMU() : bno(Adafruit_BNO055(55, 0x29, &Wire)) {}

bool IMU::init() {
  return IMU::bno.begin();
}

bool IMU::getOffestsData(uint8_t* buf) {
  Serial.printf("Getting offests\n");
  buf[0] = 100;
  uint8_t* addr = buf + 1;
  return IMU::bno.getSensorOffsets(*(adafruit_bno055_offsets_t*)addr);
}

void IMU::setOffsetsData(uint8_t* buf) {
  if (buf[0] == 100) {
    return;
  }

  Serial.printf("Setting offests\n");
  IMU::bno.setSensorOffsets(*(adafruit_bno055_offsets_t*)buf);
}

bool IMU::getNorthHeading(double lat, double lon, double alt, double* out) {
  geomag::Vector position = geomag::geodetic2ecef(lat, lon, alt);
  geomag::Vector magField = geomag::GeoMag(2022.5, position, geomag::WMM2020);
  geomag::Elements magneticFieldHeadingDiff = geomag::magField2Elements(magField, lat, lon);

  uint8_t sys, gyro, accel, mag = 0;
  bno.getCalibration(&sys, &gyro, &accel, &mag);
  if (mag != 3 && gyro != 3) {
    Serial.printf("Sys: %d, gyro: %d, accel: %d, mag: %d\n", sys, gyro, accel, mag);
    return false;
  }

  sensors_event_t orientationData;
  if (!bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER)) {
    return false;
  }

  Serial.printf("heading: %f, decl: %f\n", orientationData.orientation.heading,
    magneticFieldHeadingDiff.declination);
  *out =  orientationData.orientation.x - magneticFieldHeadingDiff.declination;

  return true;
}
