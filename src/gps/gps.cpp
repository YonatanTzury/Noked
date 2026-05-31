#include "gps.h"
// Make sure invalid accures only in the benning

#include <HardwareSerial.h>
HardwareSerial GPS_Serial(1);

void GPS::init(int rx, int tx)
{
    pinMode(21, OUTPUT);
    GPS_Serial.begin(38400, SERIAL_8N1, rx, tx);
}

void GPS::stop()
{
    GPS_Serial.end();
}

bool GPS::update()
{
    if (!GPS_Serial.available())
    {
        return false;
    }
    char serialRead = GPS_Serial.read();

    GPS::gps.encode(serialRead);

    return true;
}

bool GPS::getTime(double *out)
{
    if (GPS::rawGetTime(out))
    {
        GPS::lastUpdated = millis();
        GPS::baseTime = *out;
    }

    if (GPS::lastUpdated == -1)
    {
        return false;
    }

    return (millis() - GPS::lastUpdated) + GPS::baseTime;
}

bool GPS::rawGetTime(double *out)
{
    if (!GPS::gps.time.isValid())
    {
        return false;
    }

    *out = (double)(GPS::gps.time.value());

    return true;
}

bool GPS::getLocation(Location *out)
{
    if (!GPS::gps.location.isValid())
    {
        return false;
    }

    if (GPS::gps.location.age() > MAX_VALID_TIMEOUT)
    {
        return false;
    }

    out->lat = GPS::gps.location.lat();
    out->lon = GPS::gps.location.lng();

    return true;
}

bool GPS::getAltitude(double *out)
{
    if (!GPS::gps.altitude.isValid())
    {
        return false;
    }

    if (GPS::gps.altitude.age() > MAX_VALID_TIMEOUT)
    {
        return false;
    }

    *out = GPS::gps.altitude.meters();

    return true;
}

float GPS::getSatellites()
{
    return GPS::gps.satellites.value();
}

/*
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;
HardwareSerial GPS_Serial_2(1);

#define GPS_RX D7 // XIAO D6
#define GPS_TX D6 // XIAO D7

void setup()
{
  Serial.begin(9600);
  Serial.println("GPS starting...");
  delay(1000);

  GPS_Serial_2.begin(38400, SERIAL_8N1, GPS_RX, GPS_TX);
}

void loop()
{
  Serial.println("Reading GPS data...");
  Serial.printf("Available bytes: %d\n", GPS_Serial_2.available());
  while (GPS_Serial_2.available())
  {
    // Serial.println("Got data from GPS");
    char serialRead = GPS_Serial_2.read();
    Serial.printf("%c", serialRead);
    gps.encode(serialRead);
  }

  Serial.printf("\nGPS location updated: %d\n", gps.location.isUpdated());
  Serial.print("Lat: ");
  Serial.println(gps.location.lat(), 6);
  Serial.print("Lng: ");
  Serial.println(gps.location.lng(), 6);
  Serial.print("Satellites: ");
  Serial.println(gps.satellites.value());
  delay(1000);
}
*/