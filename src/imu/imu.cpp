#include "../imu_gy85/IMUGY85.h"
#include "imu.h"

IMUGY85 imu;
double ax, ay, az, gx, gy, gz, mx, my, mz, roll, pitch, yaw;
int lastTime = 0;

void setupIMU()
{
  imu.init();
}

void printAccel()
{
  imu.getAcceleration(&ax, &ay, &az);
  Serial.print("Accel:");
  Serial.print("\t");
  Serial.print(ax);
  Serial.print("\t");
  Serial.print(ay);
  Serial.print("\t");
  Serial.print(az);
  Serial.print("\t");
  Serial.println();
}

void printGyro()
{
  imu.getGyro(&gx, &gy, &gz);
  Serial.print("Gyro:");
  Serial.print("\t");
  Serial.print(gx);
  Serial.print("\t");
  Serial.print(gy);
  Serial.print("\t");
  Serial.print(gz);
  Serial.print("\t");
  Serial.println();
}

void printMag()
{
  imu.getMag(&mx, &my, &mz);
  Serial.print("Mag:");
  Serial.print("\t");
  Serial.print(mx);
  Serial.print("\t");
  Serial.print(my);
  Serial.print("\t");
  Serial.print(mz);
  Serial.print("\t");
  Serial.println();
}
void printRollPitchYaw()
{
  roll = imu.getRoll();
  pitch = imu.getPitch();
  yaw = imu.getYaw();
  Serial.print(pitch);
  Serial.print("\t");
  Serial.print(roll);
  Serial.print("\t");
  Serial.print(yaw);
  Serial.print("\t");
  Serial.println();
}

void loopIMU()
{
  imu.update();

  if (millis() - lastTime > 1000)
  {
    Serial.println();
    roll = imu.getRoll();
    pitch = imu.getPitch();
    yaw = imu.getYaw();
    Serial.print(pitch);
    Serial.print("\t");
    Serial.print(roll);
    Serial.print("\t");
    Serial.print(yaw);
    Serial.print("\t");
    Serial.println();
    printAccel();
    printGyro();
    printMag();
    // printRollPitchYaw();
    lastTime = millis();
  }

  delay(10);
}