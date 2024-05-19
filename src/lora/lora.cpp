#include <Arduino.h>
#include "lora.h"

void Lora::init(int nss, int rst, int dio0) {
  Lora::_lora.setPins(nss, rst, dio0);
  Lora::_lora.enableCrc();

  while (!Lora::_lora.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }
  Lora::_lora.setSyncWord(0xA5);
  Serial.println("Lora::_lora Initializing OK!");
}

void Lora::send(const uint8_t* buffer, size_t size) {
  Serial.println("Sennding");
  Lora::_lora.beginPacket();
  Lora::_lora.write(buffer, size);
  Lora::_lora.endPacket();
  Serial.println("Sent");
}

byte* Lora::read() {
  byte* buffer = NULL;
  int packetSize = Lora::_lora.parsePacket();
  Serial.printf("Racieve packet in size: %d\n", packetSize);
  if (packetSize > 0) {
    buffer = (byte *)malloc(packetSize);
    Serial.printf("Reading packet");
    Lora::_lora.readBytes(buffer, packetSize);
    Serial.printf("Read packet: %s\n", buffer);
  }

  return buffer;
}
