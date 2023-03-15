#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

int prevMS = 0;
const int numSample = 119;
int sampleRead = 0;
float threshold = 2.5;
bool isSampling = false;
float aX, aY, aZ, gX, gY, gZ;
// int 

const char service_uuid[128] = "f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5";
const char characteristic_uuid[128] = "2f925c9d-0a5b-4217-8e63-2d527c9211c1";
const char * device_name = "Nano33BLE (Left)";
// const char * device_name = "Nano33BLESense (Right)";
// Set the service and characteristic:
BLEService gestureService(service_uuid);

// BLE Characteristics
// Syntax: BLE<DATATYPE>Characteristic <NAME>(<UUID>, <PROPERTIES>, <DATA LENGTH>)
// eg. BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);
BLEStringCharacteristic gesturechar(characteristic_uuid, BLERead | BLENotify, 20);

String gesture;

void readValues();

void setup() {

  Serial.begin(9600);
  
  pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

  if (!BLE.begin()) {
    Serial.println("BLE failed to initialise");
    delay(500);
    while(1);
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // Set advertised name and service:
  BLE.setLocalName(device_name);
  BLE.setAdvertisedService(gestureService);
  gestureService.addCharacteristic(gesturechar);

  // Add service
  BLE.addService(gestureService);

  // start advertising 
  BLE.advertise();

  Serial.print(device_name);
  Serial.println(" is now active, waiting for connections...");
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("Connected to central: "); Serial.println(central.address());

    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      // read sensor value
      readValues();
    }
  }

  digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED:

  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}

int tempo = 120;

void readValues() {
  // Serial.println(String(1000000 / (micros() - prevMS)) + "Hz");
  // prevMS = micros();
  
  if (!isSampling) {
    IMU.accelerationAvailable();
    IMU.readAcceleration(aX, aY, aZ);
    float aSum = fabs(aX) + fabs(aY) + fabs(aZ);

    if (aSum > threshold) isSampling = true;
  } else {
    if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
      IMU.readAcceleration(aX, aY, aZ);
      IMU.readGyroscope(gX, gY, gZ);

      // Copy value to input buffer (tensor)
      // since sampling rate is 119Hz, there are 119*6 =714 inputs
      // normalise acceleromter data [-4, +4]
      // normalise gyroscope data [-2000, +2000]
      input->data.f[sampleRead * 6 + 0] = (aX + 4) / 8;
      input->data.f[sampleRead * 6 + 1] = (aY + 4) / 8;
      input->data.f[sampleRead * 6 + 2] = (aZ + 4) / 8;
      input->data.f[sampleRead * 6 + 3] = (gX + 2000) / 4000;
      input->data.f[sampleRead * 6 + 4] = (gY + 2000) / 4000;
      input->data.f[sampleRead * 6 + 5] = (gZ + 2000) / 4000;

      sampleRead++;

      if (sampleRead == numSample) {
        sampleRead = 0;
        isReading = false;

        // Run inference
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk) {
          MicroPrintf("Invoke failed");
          return;
        }

        // gesture = "snare";
        // gesturechar.writeValue(gesture);

        for (int i=0; i < NUM_GESTURE; i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.println(output->data.f[i], 6); // Read predicted y value from output buffer (tensor)
        }
        Serial.println();
      }
    }
  }
  
}
