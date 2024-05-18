#include <TinyGPSPlus.h>
#include <NMEAGPS.h>

#define RXD2 16
#define TXD2 17

TinyGPSPlus gps;
NMEAGPS neogps; // This parses the GPS characters
gps_fix fix;

void updateSerial()
{
  delay(500);
  while (Serial.available())
  {
    Serial2.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (Serial2.available())
  {
    Serial.write(Serial2.read()); // Forward what Software Serial received to Serial Port
  }
}

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

bool displayNeoGPSInfo()
{
  fix = neogps.read();

  if (!fix.valid.location)
  {
    return false;
  }

  Serial.println("*** Neo GPS Data ***");

  double lat = fix.latitude();
  double lng = fix.longitude();

  Serial.print(F("Location: "));
  Serial.print(lat, 6);
  Serial.print(',');
  Serial.print(lng, 6);
  Serial.printf(" [https://www.google.com/maps/search/%lf,%lf]", lat, lng);
  Serial.print(F(", Altitude: "));
  Serial.print(fix.altitude());
  Serial.println();

  Serial.println("=== Neo GPS Data ===");

  return true;
}

void initGPS()
{
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(3000);
  Serial.println("Running...");
}

bool use = false;

void loopGPS()
{
  if (Serial2.available())
  {
    char serialRead = Serial2.read();
    bool valid = false;

    if (use) {
      gps.encode(serialRead);
      valid = displayTinyGPSInfo();
    } else {
      neogps.handle(serialRead);
      valid = displayNeoGPSInfo();
    }

    if (valid) {
      use = !use;
      delay(1000);
    }
  }
}