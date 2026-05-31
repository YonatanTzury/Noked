#include <Arduino.h>
#include <FastLED.h>

// #include "manager/manager.h"
#include "imu/imu.h"
#include "gps/gps.h"
#include "lora/lora.h"
#include <Adafruit_PCF8574.h>
#include <Wire.h>

// Manager manager;
#define IMU_SCL D5
#define IMU_SDA D4

#define GPS_RX D7
#define GPS_TX D6

#define LORA_DIO0 D0
#define LORA_NSS D1
#define LORA_RST D2

Adafruit_PCF8574 pcf;

IMU imu2;
GPS gps2;
Lora lora2 = Lora(FSPI);

#define SMALL_RING_AMOUNT_OF_LEDS 18
#define BIG_RING_AMOUNT_OF_LEDS 36
CRGB ledsRing[SMALL_RING_AMOUNT_OF_LEDS + BIG_RING_AMOUNT_OF_LEDS];

int find_pcf_index()
{
  Wire.begin();
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");
  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address != 0x29)
      {
        return address;
      }
    }
  }

  return -1;
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Begin setup!!!");
  delay(2000);

  int pcf_index = find_pcf_index();
  if (pcf_index == -1)
  {
    Serial.println("Couldn't find PCF port");
    while (1)
      ;
  }

  if (!pcf.begin(pcf_index, &Wire))
  {
    Serial.println("Couldn't find PCF8574");
    while (1)
      ;
  }

  // pcf.pinMode(0, OUTPUT);
  // pcf.digitalWrite(0, LOW);
  // Serial.println("pin 0 set to output high - starting GPS");

  // pinMode(D3, OUTPUT);
  // digitalWrite(D3, LOW);

  FastLED.addLeds<WS2812, D3, GRB>(ledsRing, SMALL_RING_AMOUNT_OF_LEDS + BIG_RING_AMOUNT_OF_LEDS);

  if (!imu2.init(IMU_SDA, IMU_SCL))
  {
    Serial.println("Failed to init IMU");
    return;
    while (1)
      ;
  }

  gps2.init(GPS_RX, GPS_TX);
  Serial.println("[+] GPS initialized");

  pcf.pinMode(3, OUTPUT);

  // perform reset
  pcf.digitalWrite(3, LOW);
  delay(10);
  pcf.digitalWrite(3, HIGH);
  delay(10);

  if (!lora2.init(LORA_NSS, -1 /*LORA_RST*/, LORA_DIO0))
  {
    Serial.println("Failed to init Lora");
    while (1)
      ;
  }
  Serial.println("[+] Lora initialized");

  // Error err = manager.init();
  // if (err != SUCCESS) {
  //   Serial.printf("Error init manager: %d\n", err);
  //   return;
  // }

  pinMode(D2, INPUT);
  analogSetPinAttenuation(D2, ADC_11db);

  Serial.println("[+] Initialization finished");
}

uint32_t number = 0;

bool highOrLow = LOW;
int prevBigIndex = -1;
int prevSmallIndex = -1;
int prev_pos = -1;
int pos = 0;

void loop()
{

  // Turn off previous LEDs
  //     Add tail with lower opacity to the moving LED int prevSmallIndex = -1;
  for (int i = 0; i < BIG_RING_AMOUNT_OF_LEDS; i++)
  {
    for (int tail = 1; tail <= 3; tail++)
    {
      int smallTailIndex = prevSmallIndex - tail;
      int bigTailIndex = prevBigIndex - tail;
      if (smallTailIndex >= 0)
      {
        ledsRing[smallTailIndex] = CRGB(0, 128 / (tail + 1), 0); // Dimmer green
      }
      if (bigTailIndex >= 0)
      {
        ledsRing[SMALL_RING_AMOUNT_OF_LEDS + bigTailIndex] = CRGB(0, 128 / (tail + 1), 0);
      }
    }

    if (prevSmallIndex >= 0)
    {
      ledsRing[prevSmallIndex] = CRGB::Black;
    }
    if (prevBigIndex >= 0)
    {
      ledsRing[SMALL_RING_AMOUNT_OF_LEDS + prevBigIndex] = CRGB::Black;
    }

    ledsRing[i / 2] = CRGB(0, 64, 0);
    ledsRing[SMALL_RING_AMOUNT_OF_LEDS + i] = CRGB::Green;
    prevSmallIndex = i / 2;
    prevBigIndex = i;
    FastLED.show();
  }
  ledsRing[BIG_RING_AMOUNT_OF_LEDS + SMALL_RING_AMOUNT_OF_LEDS - 1] = CRGB::Black;
  ledsRing[SMALL_RING_AMOUNT_OF_LEDS - 1] = CRGB::Black;
  FastLED.show();
  if (prev_pos >= 0)
  {
    // Turn off previous position on small ring
    if (prev_pos < SMALL_RING_AMOUNT_OF_LEDS)
      ledsRing[prev_pos] = CRGB::Black;
    // Turn off previous position on big ring
    if (prev_pos < BIG_RING_AMOUNT_OF_LEDS)
      ledsRing[SMALL_RING_AMOUNT_OF_LEDS + prev_pos] = CRGB::Black;
  }

  // Set green line on both rings at the same position
  if (pos < SMALL_RING_AMOUNT_OF_LEDS)
    ledsRing[pos] = CRGB::Green;
  if (pos < BIG_RING_AMOUNT_OF_LEDS)
    ledsRing[SMALL_RING_AMOUNT_OF_LEDS + pos] = CRGB::Green;

  prev_pos = pos;
  pos = (pos + 1) % max(SMALL_RING_AMOUNT_OF_LEDS, BIG_RING_AMOUNT_OF_LEDS);
  for (int i = 0; i < SMALL_RING_AMOUNT_OF_LEDS + BIG_RING_AMOUNT_OF_LEDS; i++)
  {
    if (i < SMALL_RING_AMOUNT_OF_LEDS)
    {
      ledsRing[i] = CRGB::Blue;
      continue;
    }

    ledsRing[i] = CRGB::Red;
  }

  // if (number != 0 && number % 5 == 0)
  // {
  //   // digitalWrite(D3, HIGH);
  //   Serial.println("DISCONNECT AND CONNECT");
  //   delay(10 * 1000);
  //   // digitalWrite(D3, LOW);
  //   Serial.println("RESUME");
  // imu2.init(IMU_SDA, IMU_SCL);
  //   // lora2.init(LORA_NSS, LORA_RST, LORA_DIO0);
  // gps2.init(GPS_RX, GPS_TX);
  // }

  gps2.update();

  char recv_byte[4] = {0};
  size_t curPacketSize = 0;
  size_t recv_bytes = lora2.read((byte *)&recv_byte, sizeof(recv_byte), &curPacketSize);
  if (curPacketSize > 0)
  {
    Serial.printf("Available Packet Size: %d\n", curPacketSize);
  }
  if (recv_bytes > 0)
  {
    Serial.printf("Received %d bytes, raw value: ", recv_bytes);
    for (int i = 0; i < recv_bytes; i++)
    {
      Serial.printf("%02X ", ((uint8_t *)recv_byte)[i]);
    }
    Serial.println("");

    float f = 0;
    // Safely copy the 4 bytes into the float variable
    memcpy(&f, recv_byte, sizeof(f));
    Serial.printf("Received voltage: %f\n", f);

    int rssi = lora2._lora.rssi();
    Serial.printf("Lora RSSI: %d\n", rssi);
    return;
  }

  EVERY_N_MILLISECONDS(1000)
  {
    Serial.println("Heartbeat");

    float miliVoltRead = ((analogReadMilliVolts(D2) / 1000.0f) * 2);
    Serial.printf("Battery: %f\n", miliVoltRead);

    // highOrLow = !highOrLow;
    // digitalWrite(D3, highOrLow);

    Serial.printf("Sending number: %d\n", number);
    // char msg[] = "hello world";
    char msg[5] = {0};
    memcpy(msg, &miliVoltRead, sizeof(float));
    size_t sent_bytes = lora2.send((byte *)&msg, sizeof(float));
    Serial.printf("Sent %d bytes\n", sent_bytes);

    number++;

    Serial.print("Satellites: ");
    Serial.println(gps2.getSatellites());
    Location tmpLocation = {0};
    if (!gps2.getLocation(&tmpLocation))
    {
      Serial.println("Failed to get Location");
      return;
    }
    Serial.printf("Lat: %f, Lon: %f\n", tmpLocation.lat, tmpLocation.lon);

    double time;
    if (!gps2.getTime(&time))
    {
      Serial.println("Failed to get Time");
      return;
    }
    Serial.printf("Time: %f\n", time);

    double alt;
    if (!gps2.getAltitude(&alt))
    {
      return;
    }
    Serial.printf("Altitude: %f\n", alt);

    // Serial.println("waiting for 10s");
    // delay(10000);
    // Serial.println("reset GPS pins");
    // gps2.stop();
    // // pinMode(GPS_RX, OUTPUT);
    // // pinMode(GPS_TX, OUTPUT);
    // // digitalWrite(GPS_RX, LOW);
    // // digitalWrite(GPS_TX, LOW);

    // delay(100);

    // Serial.println("stoping gps voltage");
    // digitalWrite(D3, HIGH);

    // delay(120 * 1000);
    // Serial.println("starting gps voltage");
    // digitalWrite(D3, LOW);
    // delay(100);
    // Serial.println("Re-initializing GPS");
    // gps2.init(GPS_RX, GPS_TX);
    // Serial.println("GPS re-initialized");

    double out = 10000;
    if (!imu2.getNorthHeading(tmpLocation.lat, tmpLocation.lon, alt, &out))
      digitalWrite(D3, HIGH);
    if (!imu2.getNorthHeading(30, 30, 30, &out))
    {
      Serial.println("Failed to get North Heading");
      return;
    }
    Serial.printf("IMU Read Complete: %f\n", out);
    digitalWrite(D3, LOW);

    Serial.printf("North Heading: %f\n", out);
  }
}

/*
esp + lora = 25ma
esp + lora + IMU = 35ma
esp + lora + IMU + GPS = 80-120ma

GPS - 45ma
IMU - 10ma

ESP - 10ms
ESP + GPS (no TX RX) - 65
ESP + GPS (with TX RX) - 65
ESP + TX RX only - 23
ESP + LORA READS - 40
ESP + LORA WRITE - 120
ESP + LORA IDLE with or without init - 27
ESP + IMU READING - 50
ESP + IMU IDLE - 37

*/