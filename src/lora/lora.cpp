#include <Arduino.h>
#include "lora.h"

bool Lora::init(int nss, int rst, int dio0) {
  Lora::_lora.setPins(nss, rst, dio0);
  int res = Lora::_lora.begin(433E6);
  Lora::_lora.enableCrc();
  Lora::_lora.setSyncWord(0x34);
  return res;
}

int Lora::rssi() {
  return Lora::_lora.rssi();
}

size_t Lora::send(const uint8_t* buffer, size_t size) {
  // TODO: handle errors
  if (Lora::_lora.beginPacket() == 0) {
    return 0;
  }
  size_t result = Lora::_lora.write(buffer, size);
  if (result != size) {
    return 0;
  }

  if (Lora::_lora.endPacket() == 0) {
    return 0;
  }

  return result;
}

size_t Lora::read(byte* buffer, size_t size, size_t* avilablePacketSize) {
  int packetSize = Lora::_lora.parsePacket();
  *avilablePacketSize = packetSize;

  if (packetSize == 0 || packetSize > size) {
    return 0;
  }

  return Lora::_lora.readBytes(buffer, packetSize);
}
