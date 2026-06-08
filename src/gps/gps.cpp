#include "gps.h"
// Make sure invalid accures only in the benning

#include <HardwareSerial.h>
HardwareSerial GPS_Serial(1);

bool GPS::init(int rx, int tx) {
  GPS_Serial.begin(38400, SERIAL_8N1, rx, tx);
  for (int i = 0; i < 10; i++) {
    if (GPS::rawGetTime(&(GPS::baseTime))) {
      GPS::lastUpdated = millis();
      return true;
    }

    delay(1000);
  }

  return false;
}

void GPS::stop() {
  GPS_Serial.end();
}

bool GPS::update() {
  if (!GPS_Serial.available()) {
    return false;
  }
  char serialRead = GPS_Serial.read();

  GPS::gps.encode(serialRead);

  return true;
}

double GPS::getTime() {
  double out;
  if (GPS::rawGetTime(&out)) {
    GPS::lastUpdated = millis();
    GPS::baseTime = out;
    return out;
  }

  return (millis() - GPS::lastUpdated) + GPS::baseTime;
}

bool GPS::rawGetTime(double* out) {
  if (!GPS::gps.time.isValid()) {
    return false;
  }

  *out = (double)(GPS::gps.time.value());

  return true;
}

bool GPS::getLocation(Location* out) {
  if (!GPS::gps.location.isValid()) {
    return false;
  }

  if (GPS::gps.location.age() > MAX_VALID_TIMEOUT) {
    return false;
  }

  out->lat = GPS::gps.location.lat();
  out->lon = GPS::gps.location.lng();

  return true;
}

bool GPS::getAltitude(double* out) {
  if (!GPS::gps.altitude.isValid()) {
    return false;
  }

  if (GPS::gps.altitude.age() > MAX_VALID_TIMEOUT) {
    return false;
  }

  *out = GPS::gps.altitude.meters();

  return true;
}

float GPS::getSatellites() {
  return GPS::gps.satellites.value();
}
