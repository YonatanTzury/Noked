#include <Arduino.h>
#include <utility/imumaths.h>

#include "imu.h"
#include "XYZgeomag.hpp"

IMU::IMU() : _wire(TwoWire(1)), bno(Adafruit_BNO055(55, 0x29, &_wire)) {}

bool IMU::init() {
    if (!IMU::bno.begin()) {
        /* There was a problem detecting the BNO055 ... check your connections */
        Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
        return false;
    }

    return true;
}

double IMU::getNorth(double lat, double lon, double alt) {
//   geomag::Vector position = geomag::geodetic2ecef(lat, lon, alt);
//   geomag::Vector mag_field = geomag::GeoMag(2022.5, position, geomag::WMM2020);
//   geomag::Elements out = geomag::magField2Elements(mag_field, lat, lon);

//   return imu.getYaw() + out.declination;
    return 0.0;
}
