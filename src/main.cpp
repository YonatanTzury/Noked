#include <Arduino.h>

#define LoraTest 0
#define GPSTest 1
#define IMUTest 0

#ifdef GPSTest
#include "gps/gps.h"
#endif

void setup() {
  #ifdef LoraTest
  #endif

  #ifdef GPSTest
  initGPS();
  #endif

  #ifdef IMUTest
  #endif
}

void loop() {
  #ifdef LoraTest
  #endif

  #ifdef GPSTest
  loopGPS();
  #endif

  #ifdef IMUTest
  #endif
}
