#include <Arduino.h>

#include "manager/manager.h"

Manager manager;

void setup() {
  Serial.begin(115200);
  Serial.println("Begin setup!!!");
  delay(2000);

  Error err = manager.init();
  if (err != SUCCESS) {
    Serial.printf("Manager init failed: %d\n", err);
    while (1)
      ;
  }

  Serial.println("[+] Initialization finished");
}

void loop() {
  manager.loop();
}
