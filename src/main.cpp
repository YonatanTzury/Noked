#include <Arduino.h>

// #define LoraTest
#define GPSTest
// #define IMUTest

#ifdef GPSTest
#include "gps/gps.h"
#endif

#ifdef IMUTest
#include "imu/imu.h"
#endif

void setup() {
  Serial.begin(9600);

  #ifdef LoraTest
  #endif

  #ifdef GPSTest
  setupGPS();
  #endif

  #ifdef IMUTest
  setupIMU();
  #endif
}

void loop() {
  #ifdef LoraTest
  #endif

  #ifdef GPSTest
  loopGPS();
  #endif

  #ifdef IMUTest
  loopIMU();
  #endif
}
