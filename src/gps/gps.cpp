#include "gps.h"

#define RXD2 16
#define TXD2 17

// Make sure invalid accures only in the benning

void GPS::init() {
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // TODO maybe also get location and altitude until valid
  while (!GPS::rawGetTime(&(GPS::baseTime))) {
    delay(1000);
  }
  GPS::lastUpdated = millis();
}

bool GPS::update() {
  if (!Serial2.available()) {
    return false;
  }
  char serialRead = Serial2.read();

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
