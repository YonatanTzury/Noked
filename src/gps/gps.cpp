#include "gps.h"

// Make sure invalid accures only in the benning

void GPS::init(int rx, int tx) {
  Serial2.begin(9600, SERIAL_8N1, rx, tx);
}

bool GPS::update() {
  if (!Serial2.available()) {
    return false;
  }
  char serialRead = Serial2.read();

  GPS::gps.encode(serialRead);

  return true;
}

bool GPS::getTime(double* out) {
  if (GPS::rawGetTime(out)) {
    GPS::lastUpdated = millis();
    GPS::baseTime = *out;
  }

  if (GPS::lastUpdated == -1) {
    return false;
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
