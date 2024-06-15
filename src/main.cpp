#include <Arduino.h>

#include "manager\manager.h"

Manager manager;

void setup() {
  Serial.begin(9600);

  manager.init();

  Serial.printf("Begin finished");
}

void loop() {
  manager.loop();
}
