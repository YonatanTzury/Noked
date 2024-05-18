#include <TinyGPSPlus.h>

#define RXD2 16
#define TXD2 17

TinyGPSPlus gps;

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

void displayInfo()
{
	Serial.print(F("Location: "));
	if (gps.location.isValid())
	{
    double lat = gps.location.lat();
    double lng = gps.location.lng();

		Serial.print(lat, 6);
		Serial.print(F(","));
		Serial.print(lng, 6);
    Serial.print(F("Sats: "));
    Serial.print(gps.satellites.value());
    Serial.printf(" [https://www.google.com/maps/search/%lf,%lf]", lat, lng);
	}
	else
	{
		Serial.print(F("INVALID"));
	}
  Serial.println();
}

void initGPS()
{
	Serial.begin(9600);
	Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
	delay(3000);
	Serial.println("Running...");
}

void loopGPS()
{
	if (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      displayInfo();
      delay(500);
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}