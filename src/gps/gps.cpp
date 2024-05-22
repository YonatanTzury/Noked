#include <TinyGPSPlus.h>

#include "gps.h"

#define RXD2 16
#define TXD2 17

void GPS::init() {
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while (!GPS::rawGetTime(&(GPS::baseTime))) {
    delay(1000);
  }
  GPS::lastUpdated = millis();
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
  if (!Serial2.available()) {
    return false;
  }
  char serialRead = Serial2.read();

  TinyGPSPlus gps;
  gps.encode(serialRead);
  if (!gps.time.isValid()) {
    return false;
  }

  *out = (double)gps.time.value();

  return true;
}

bool GPS::getLocation(Location* out) {
  if (!Serial2.available()) {
    return false;
  }
  char serialRead = Serial2.read();

  TinyGPSPlus gps;
  gps.encode(serialRead);
  if (!gps.location.isValid()) {
    return false;
  }

  out->lat = gps.location.lat();
  out->lon = gps.location.lng();

  return true;
}
