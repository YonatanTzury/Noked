#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

class Lora {
public:
  bool init(int nss, int rst, int dio0);
  size_t send(const uint8_t* buffer, size_t size);
  size_t read(byte* buffer, size_t size, size_t* avilablePacketSize);
  int rssi();

private:
  LoRaClass _lora;
};
