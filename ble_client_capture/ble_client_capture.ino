#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

volatile int prevMS = 0;
volatile bool isSampling = false;
const int tempo = 120;
const int interval = 500; // tempo=120 => 500ms
const int numSample = 60;
int sampleRead = 0;
// float threshold = 1.8;
float aX, aY, aZ, gX, gY, gZ;
float coolDown = 0;

volatile int mode = 0; // 0: demo, 1: data collection

const char service_uuid[128] = "f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5";
const char gesture_char_uuid[128] = "2f925c9d-0a5b-4217-8e63-2d527c9211c1";
// const char img_char_uuid[128] = "f8edf338-6bbd-4c3b-bf16-d8d2b6cdaa6e";

const char * device_name = "Drum_L";
// const char * device_name = "Drum_R";


// Set the service and characteristic:
BLEService service(service_uuid);

// BLE Characteristics
// Syntax: BLE<DATATYPE>Characteristic <NAME>(<UUID>, <PROPERTIES>, <DATA LENGTH>)
BLEStringCharacteristic gestureChar(gesture_char_uuid, BLERead | BLEWrite | BLENotify, 47);
// BLEStringCharacteristic imuChar(img_char_uuid, BLERead | BLEWrite | BLENotify, 20);


void readValues();

void inferSamples();

static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic);

String gesture;

void setup() {
  Serial.begin(9600);

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
  BLE.setAdvertisedService(service);
  service.addCharacteristic(gestureChar);
  // service.addCharacteristic(imuChar);

  // Add service
  BLE.addService(service);
  // imuChar.setEventHandler(BLEWritten, onReceiveMsg);
  gestureChar.setEventHandler(BLEWritten, onReceiveMsg);

  // start advertising 
  BLE.advertise();
  Serial.print(device_name);
  Serial.println(" is now active, waiting for connections...");

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  // pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);
  // digitalWrite(LEDB, HIGH);
}

float samples[numSample * 6];
bool ledOn = false;
float bf[6];

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    while (central.connected()) {
      if (ledOn == 0) {  // turn on the LED to indicate the connection:
        // digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, LOW);
        ledOn = 1;
        Serial.print("Connected to central: "); Serial.println(central.address());
      } 
      // data collection mode
      if (mode == 1) { // data collection
          if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
            IMU.readAcceleration(aX, aY, aZ);
            IMU.readGyroscope(gX, gY, gZ);
            // normalise accelerometer and gyroscope abs value to range: [0, 1]
            bf[0] = aX / 4;
            bf[1] = aY / 4;
            bf[2] = aZ / 4;
            bf[3] = gX / 2000;
            bf[4] = gY / 2000;
            bf[5] = gZ / 2000;
            gestureChar.writeValue(String(bf[0])+","+String(bf[1])+","+String(bf[2])+","+String(bf[3])+","+String(bf[4])+","+String(bf[5]));
        }
        continue;
      }

      // demo mode
      if (sampleRead < numSample) {
        if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
          IMU.readAcceleration(aX, aY, aZ);
          IMU.readGyroscope(gX, gY, gZ);
          
          // normalise accelerometer and gyroscope abs value to range: [0, 1]
          bf[0] = aX / 4;
          bf[1] = aY / 4;
          bf[2] = aZ / 4;
          bf[3] = gX / 2000;
          bf[4] = gY / 2000;
          bf[5] = gZ / 2000;
          sampleRead++;
        }
      }

      if (millis() - prevMS >= interval) {
        // data padding
          for (int i = sampleRead; i < numSample; i++) {
            samples[i * 6] = 0;
            samples[i * 6 + 1] = 0;
            samples[i * 6 + 2] = 0;
            samples[i * 6 + 3] = 0;
            samples[i * 6 + 4] = 0;
            samples[i * 6 + 5] = 0;
          }
          
          // // inference
          gestureChar.writeValue("3");

          sampleRead = 0;
          isSampling = false;
          prevMS = millis();
      }
    }
  }

  if (ledOn == 1) {
    // digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    ledOn = 0;
    Serial.print("Disconnect: "); Serial.println(central.address());
  } 

}

// const int tempos[3] = {60, 120, 180};
// char * tempoIndex;
// int tempo;
static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic) {
  char indexStr[2] = {' ', '\0'};
  strncpy(indexStr, (char *) characteristic.value(), 1);
  mode = atoi(indexStr);
  Serial.println(String(mode) + ", " + indexStr);
}

void inferSamples() {

}

// void sampleData() {
//   if (!isSampling) {
//     IMU.accelerationAvailable();
//     IMU.readAcceleration(aX, aY, aZ);
//     float aSum = fabs(aX) + fabs(aY) + fabs(aZ);
//     // maxVal = aSum > maxVal ? aSum : maxVal;
//     // Serial.println("thrsh: " + String(aSum) + " ");
//     if (aSum > threshold) isSampling = true;
//   } else {
//     if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
//       IMU.readAcceleration(aX, aY, aZ);
//       IMU.readGyroscope(gX, gY, gZ);
//       Serial.print(String(aX) + "," + String(aY) + "," + String(aZ) + "," + String(gX) + "," + String(gY) + "," + String(gZ) + "\n");
//       sampleRead++;

//       if (sampleRead == numSample) {
//         sampleRead = 0;
//         isSampling = false;
//         coolDown = 200;
//         float prevMS = micros();
//         while (coolDown > 0) {
//           coolDown -= (micros() - prevMS) / 1000;
//           prevMS = micros();
//         }
//         coolDown = 0;
//       }
//     }
//   }
// }
