#include <Arduino.h>

#include "manager\manager.h"

Manager manager;

void setup() {
  Serial.begin(9600);
  delay(1000);
  // while(!Serial);
  Serial.println("Init");

  Error err = manager.init();
  if (err != SUCCESS) {
    Serial.printf("Error init manager: %d\n", err);
    return;
  }

  Serial.printf("Begin finished\n");

}

void loop() {
  manager.loop();
}
