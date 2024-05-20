#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

#define lora_localAddress 0x01

class Lora {
 public:
  void init(int nss, int rst, int dio0, int sck, int miso, int mosi, SPIClass& spi);
  void send(const uint8_t* buffer, size_t size);
  byte* read();

 private:
  LoRaClass _lora;
  int timeout = 200;
};
