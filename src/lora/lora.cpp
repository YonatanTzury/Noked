#include <Arduino.h>
#include "lora.h"

void Lora::init(int nss, int rst, int dio0, int sck, int miso, int mosi, SPIClass& spi) {
  Lora::_lora.setPins(nss, rst, dio0);
  Lora::_lora.setSPI(spi);
  // Lora::_lora.enableCrc();

  while (!Lora::_lora.begin(433E6, sck, miso, mosi, nss)) {
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

byte* Lora::read() {
  byte* buffer = NULL;
  int packetSize = Lora::_lora.parsePacket();
  if (packetSize > 0) {
    buffer = (byte *)malloc(packetSize);
    Lora::_lora.readBytes(buffer, packetSize);
  }

  return buffer;
}
