#include <Arduino.h>

#define LoraTest 1
#define GPSTest 0
#define IMUTest 0

#ifdef LoraTest

#include "lora\lora.h"
#define lora1_rst 32
#define lora1_nss 15
#define lora1_dio0 26
#define lora1_sck 14
#define lora1_miso 12
#define lora1_mosi 13

Lora lora;

const uint8_t buffer[10] = {'S', 't','e', 'v', 'e', 0};

#endif
// put function declarations here:

void setup() {
 // put your setup code here, to run once:
  #ifdef LoraTest
  Serial.begin(9600);

  SPIClass _hspi = SPIClass(HSPI);

  lora.init(lora1_nss, lora1_rst, lora1_dio0, lora1_sck, lora1_miso, lora1_mosi, _hspi);

  int lastSend;
  int counter;

  while (true) {
    if (millis() - lastSend > 1000) {
      lastSend = millis();
      counter++;
      lora.send((byte*)&counter, sizeof(counter));
      delay(10);
    }

    byte* res = lora.read();
    if (res != NULL) {
      Serial.printf("Recieved from lora: %d\n", *(int*)res);
      free(res);
    }
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
