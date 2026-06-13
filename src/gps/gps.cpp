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

// Days from 1970-01-01 to the given civil date (proleptic Gregorian).
// Howard Hinnant's days_from_civil algorithm.
static int32_t daysFromCivil(int32_t y, uint32_t m, uint32_t d) {
  y -= m <= 2;
  int32_t era = (y >= 0 ? y : y - 399) / 400;
  uint32_t yoe = (uint32_t)(y - era * 400);
  uint32_t doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  uint32_t doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + (int32_t)doe - 719468;
}

uint32_t GPS::getTime() {
  uint32_t out;
  if (GPS::rawGetTime(&out)) {
    GPS::lastUpdated = millis();
    GPS::baseTime = out;
    return out;
  }

  if (GPS::baseTime == 0) {
    return 0;
  }

  // No fresh fix: extrapolate from the last known epoch using elapsed millis.
  // The unsigned subtraction is rollover-safe across the millis() wrap.
  return GPS::baseTime + (millis() - GPS::lastUpdated) / 1000;
}

bool GPS::rawGetTime(uint32_t* out) {
  if (!GPS::gps.time.isValid() || !GPS::gps.date.isValid()) {
    return false;
  }

  int32_t days = daysFromCivil(GPS::gps.date.year(), GPS::gps.date.month(),
                               GPS::gps.date.day());
  *out = (uint32_t)days * 86400u + GPS::gps.time.hour() * 3600u +
         GPS::gps.time.minute() * 60u + GPS::gps.time.second();

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
