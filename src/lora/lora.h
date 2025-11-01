#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

class Lora {
 public:
  Lora(uint8_t spi_bus);
  bool init(int nss, int rst, int dio0);
  void send(const uint8_t* buffer, size_t size);
  size_t read(byte* buffer, size_t size);

 private:
  LoRaClass _lora;
  SPIClass _spi;
};
