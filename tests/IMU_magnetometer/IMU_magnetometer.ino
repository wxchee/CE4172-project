#include <Arduino_LSM9DS1.h>

float x, y, z;

void setup() {
  Serial.begin(9600);

  while (!Serial);

  if (!IMU.begin()) { Serial.println(F("Failed to initialize IMU!")); while (1);  }

  // Magnetometer code
   IMU.setMagnetFS(0);
   IMU.setMagnetODR(8);
   IMU.setMagnetOffset(28.376465, -22.164307, 15.271606);
   IMU.setMagnetSlope (1.163948, 1.191517, 1.096843);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (IMU.magnetAvailable()) {
    IMU.readMagnet(x, y, z);

    Serial.print(round(x));
    Serial.print('\t');
    Serial.print(round(y));
    Serial.print('\t');
    Serial.println(round(z));
  }
}
