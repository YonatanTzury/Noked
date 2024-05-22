#include <TinyGPSPlus.h>

#define RXD2 16
#define TXD2 17

TinyGPSPlus gps;

bool displayTinyGPSInfo()
{
  if (!gps.location.isValid())
  {
    return false;
  }

  Serial.println("*** Tiny GPS Data ***");

  double lat = gps.location.lat();
  double lng = gps.location.lng();

  Serial.print(F("Location: "));
  Serial.print(lat, 6);
  Serial.print(F(","));
  Serial.print(lng, 6);
  Serial.printf(" [https://www.google.com/maps/search/%lf,%lf]", lat, lng);
  Serial.println();

  Serial.println("=== Tiny GPS Data ===");

  return true;
}

void setupGPS()
{
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  delay(3000);

  Serial.println("Running...");
}

bool use = false;

void loopGPS()
{
  if (!Serial2.available())
  {
    return;
  }

  char serialRead = Serial2.read();

  gps.encode(serialRead);
  bool valid = displayTinyGPSInfo();
  if (!valid)
  {
    return;
  }

  delay(1000);
}