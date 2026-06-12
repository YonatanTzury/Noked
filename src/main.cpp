#include <Arduino.h>

#include "logger/logger.h"
#include "manager/manager.h"

Manager manager;

void setup() {
  Serial.begin(115200);
  log(INFO, "Begin setup!!!");
  delay(2000);

  Error err = manager.init();
  if (err != SUCCESS) {
    log(ERROR, "Manager init failed: %d", err);
    while (1)
      ;
  }

  log(INFO, "Initialization finished");
}

void loop() {
  manager.loop();
}
