#include <Arduino.h>

#define LoraTest
// #define GPSTest
// #define IMUTest

#ifdef LoraTest
#include "manager\manager.h"

Manager manager;
#endif
// put function declarations here:

void setup() {
 // put your setup code here, to run once:
  #ifdef LoraTest
  Serial.begin(9600);

  manager.init();

  while (true) {
    manager.loop();
  }

  #endif

  #ifdef GPSTest
  #endif

  #ifdef IMUTest
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:
  #ifdef LoraTest
  #endif

  #ifdef GPSTest
  #endif

  #ifdef IMUTest
  #endif
}

// put function definitions here:
