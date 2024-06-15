#include <Arduino.h>
#include "lora.h"

Lora::Lora(uint8_t spi_bus) : _spi(SPIClass(spi_bus)) {}

bool Lora::init(int nss, int rst, int dio0) {
  Lora::_lora.setPins(nss, rst, dio0);
  Lora::_lora.setSPI(Lora::_spi);
  Lora::_lora.enableCrc();

  return Lora::_lora.begin(433E6);
}

void Lora::send(const uint8_t* buffer, size_t size) {
  // TODO: handle errors
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
