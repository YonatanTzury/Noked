#include <Arduino.h>
#include "lora.h"

void Lora::init(int nss, int rst, int dio0, SPIClass& spi) {
  Lora::_lora.setPins(nss, rst, dio0);
  Lora::_lora.setSPI(spi);
  Lora::_lora.enableCrc();

  while (!Lora::_lora.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
  Serial.println("Lora::_lora Initializing OK!");
}

void Lora::send(const uint8_t* buffer, size_t size) {
  Lora::_lora.beginPacket();
  Lora::_lora.write(buffer, size);
  Lora::_lora.endPacket();
}

size_t Lora::read(byte* buffer, size_t size) {
  int packetSize = Lora::_lora.parsePacket();

  if (packetSize == 0 || packetSize > size) {
    return 0;
  }

  return Lora::_lora.readBytes(buffer, packetSize);
}
