#include <Arduino.h>

// #define LoraTest
// #define GPSTest
// #define IMUTest
#define TMPTest
#define INATest

#ifdef GPSTest
#include "gps/gps.h"
#endif

#ifdef IMUTest
#include "imu/imu.h"
#endif

#ifdef LoraTest
#include "manager\manager.h"

Manager manager;
#endif

#ifdef TMPTest
#endif

#ifdef INATest
// #include <Wire.h>
// TwoWire _Wire(10);

// https://learn.adafruit.com/build-a-cloud-connected-esp8266-power-meter/hardware-configuration
// https://electronicstree.com/ina219-current-sensor-with-arduino/#:~:text=INA219%20Module&text=To%20measure%20current%2C%20the%20module,the%20voltage%20across%20the%20load.
// https://cdwilson.dev/articles/understanding-the-ina219/
#include <Adafruit_INA219.h>
#define SCL 33
#define SDA 25
#define CONTROL 26
Adafruit_INA219 ina(0x40);
uint8_t i = 200;
#endif

void setup() {
  Serial.begin(9600);

  #ifdef LoraTest
  manager.init();
  #endif

  #ifdef GPSTest
  setupGPS();
  #endif

  #ifdef IMUTest
  setupIMU();
  #endif

  #ifdef TMPTest
  #endif

  #ifdef INATest
  pinMode(CONTROL, OUTPUT);

  if (!Wire.setPins(SDA, SCL)) {
    Serial.printf("Failed set pins\n");
  }
  Wire.begin();
  if (!ina.begin() ) {
    Serial.println("Could not connect. Fix and Reboot");
  }
  ina.setCalibration_16V_400mA();
  #endif

  Serial.printf("Begin finished");
}

void loop() {
  #ifdef LoraTest
  manager.loop();
  #endif

  #ifdef GPSTest
  loopGPS();
  #endif

  #ifdef IMUTest
  loopIMU();
  #endif

  #ifdef TMPTest
  #endif
  
  #ifdef INATest
  Serial.printf("Index: %d\n", i);
  analogWrite(CONTROL, i);
  i++;

  Serial.printf("Shunt Voltage (mV): %f\n", ina.getShuntVoltage_mV());
  Serial.printf("Bus Voltage (V): %f\n", ina.getBusVoltage_V());
  Serial.printf("Current: %f (mA)\n", ina.getCurrent_mA());
  Serial.printf("Power: %f (mW)\n", ina.getPower_mW());
  Serial.println();

  delay(1000);
  #endif

}
