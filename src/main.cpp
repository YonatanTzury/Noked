#include <Arduino.h>

// #include "manager/manager.h"
#include "imu/imu.h"
#include "gps/gps.h"

// Manager manager;
#define IMU_SCL D5
#define IMU_SDA D4

#define GPS_RX D7
#define GPS_TX D6

IMU imu2;
GPS gps2;

void setup()
{
  Serial.begin(115200);
  Serial.println("Begin setup");
  delay(2000);

  // if (!imu2.init(IMU_SDA, IMU_SCL))
  // {
  //   Serial.println("Failed to init IMU");
  //   return;
  // }

  gps2.init(GPS_RX, GPS_TX);

  // Error err = manager.init();
  // if (err != SUCCESS) {
  //   Serial.printf("Error init manager: %d\n", err);
  //   return;
  // }

  Serial.println("Begin finished");
}

void loop()
{
  // delay(500);
  // Serial.println("Looping");
  // double lat = 32.109333;
  // double lon = 34.855499;
  // double alt = 40.0;
  // double out;
  // if (!imu2.getNorthHeading(lat, lon, alt, &out))
  // {
  //   Serial.println("Failed to get North Heading");
  //   return;
  // }
  // Serial.printf("North Heading: %f\n", out);

  if (!gps2.update())
  {
    Serial.println("Failed to update GPS");
    return;
  }

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
}
