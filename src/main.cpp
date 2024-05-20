#include <Arduino.h>

#define LoraTest 1
#define GPSTest 0
#define IMUTest 0

#ifdef LoraTest

#include "lora\lora.h"
#define lora1_rst 32
#define lora1_nss 15
#define lora1_dio0 25
#define lora1_sck 14
#define lora1_miso 12
#define lora1_mosi 13

#define lora2_rst 22
#define lora2_nss 5
#define lora2_dio0 21
#define lora2_sck 18
#define lora2_miso 19
#define lora2_mosi 23
// TODO change dio0 to interupt capable, do we realy need dio0?
// TODO lora.end?
Lora loraSender;
Lora loraReceiver;

const uint8_t buffer[10] = {'H', 'e', 'l', 'l', 'o', 0};
#endif
// put function declarations here:

void setup() {
 // put your setup code here, to run once:
  #ifdef LoraTest
  Serial.begin(9600);
  SPIClass _hspi = SPIClass(HSPI);
  SPIClass _vspi = SPIClass(VSPI);
  loraSender.init(lora1_nss, lora1_rst, lora1_dio0, lora1_sck, lora1_miso, lora1_mosi, _hspi);
  loraReceiver.init(lora2_nss, lora2_rst, lora2_dio0, lora2_sck, lora2_miso, lora2_mosi, _vspi);
  while (true) {
    loraSender.send(buffer, 10);
    delay(1000);
    byte* res = loraReceiver.read();
    if (res != NULL) {
      Serial.printf("Recieved from lora: %s\n", res);
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
