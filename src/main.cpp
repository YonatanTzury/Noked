#include <Arduino.h>

#define LoraTest
// #define GPSTest
// #define IMUTest

#ifdef GPSTest
#include "gps/gps.h"
#endif

#ifdef IMUTest
#include "imu/imu.h"
#endif

#ifdef LoraTest
#include "manager\manager.h"

Manager manager;
#endif

void setup() {
  Serial.begin(9600);

  #ifdef LoraTest
  manager.init();
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
  manager.loop();
  #endif

  #ifdef GPSTest
  loopGPS();
  #endif

  #ifdef IMUTest
  loopIMU();
  #endif
}
