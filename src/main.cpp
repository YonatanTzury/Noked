#include <Arduino.h>

#define LoraTest 1
#define GPSTest 0
#define IMUTest 0

#ifdef LoraTest

#include "lora\lora.h"
#define lora1_rst 34
#define lora1_nss 35
#define lora1_dio0 32

#define lora2_rst 33
#define lora2_nss 25
#define lora2_dio0 26
// TODO change dio0 to interupt capable, do we realy need dio0?
// TODO lora.end?
Lora loraSender;
Lora loraReceiver;

#endif
// put function declarations here:

void setup() {
 // put your setup code here, to run once:
  #ifdef LoraTest
  loraSender.init(lora1_nss, lora1_rst, lora1_dio0);
  loraReceiver.init(lora2_nss, lora2_rst, lora2_dio0);
  uint8_t buffer[10] = {'H', 'e', 'l', 'l', 'o', 0};
  loraSender.send(buffer, 10);
  delay(1000);
  byte* res = loraReceiver.read();
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
