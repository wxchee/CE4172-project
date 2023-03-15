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


const char service_uuid[128] = "f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5";
const char gesture_char_uuid[128] = "2f925c9d-0a5b-4217-8e63-2d527c9211c1";
// const char msg_char_uuid[128] = "f8edf338-6bbd-4c3b-bf16-d8d2b6cdaa6e";

const char * device_name = "Drum_L";
// const char * device_name = "Drum_R";


// Set the service and characteristic:
BLEService service(service_uuid);

// BLE Characteristics
// Syntax: BLE<DATATYPE>Characteristic <NAME>(<UUID>, <PROPERTIES>, <DATA LENGTH>)
// eg. BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);
BLEStringCharacteristic gestureChar(gesture_char_uuid, BLERead | BLEWrite | BLENotify, 20);
// BLEStringCharacteristic gestureChar(gesture_char_uuid, BLERead | BLENotify, 20);
// BLEStringCharacteristic msgChar(msg_char_uuid, BLEWrite | BLENotify, 1);

void readValues();

void inferSamples();

static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic);

String gesture;

void setup() {
  Serial.begin(9600);
  Serial.println(BOARD_NAME);

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
  // service.addCharacteristic(msgChar);

  // Add service
  BLE.addService(service);
  
  // msgChar.setEventHandler(BLEWritten, onReceiveMsg);
  gestureChar.setEventHandler(BLEWritten, onReceiveMsg);

  // start advertising 
  BLE.advertise();

  Serial.println(" is now active, waiting for connections...");
}

float maxVal = 0;
// const numSampleLinear = numSample * 6;
float samples[numSample * 6];
bool ledOn = false;
void loop() {
  // Serial.println(String(1000000 / (micros() - prevMS)) + "Hz");
  // prevMS = micros();

  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: "); Serial.println(central.address());

    prevMS = millis();
    while (!central.connected()) {
      if (ledOn) {
        digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED:
        ledOn = 0;
      }
      Serial.println("Disconnected from central: ");
      Serial.println(central.address());
    }

    // turn on the LED to indicate the connection:
    if (!ledOn) {
      digitalWrite(LED_BUILTIN, HIGH);
      ledOn = 1;
    }    
    
    while (central.connected()) {
      // if (!isSampling) {
      //   prevMS = millis();
      //   continue;
      // };

      if (sampleRead < numSample) {
        if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
          IMU.readAcceleration(samples[sampleRead * 6], samples[sampleRead * 6 + 1], samples[sampleRead * 6 + 2]);
          IMU.readGyroscope(samples[sampleRead * 6 + 3], samples[sampleRead * 6 + 4], samples[sampleRead * 6 + 5]);
          sampleRead++;
        }
      }

      if (millis() - prevMS >= interval) {
        Serial.println("manual sample: " + String(sampleRead));
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

}

// const int tempos[3] = {60, 120, 180};
// char * tempoIndex;
// int tempo;
static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic) {
  Serial.println("t");
  isSampling = true;
  // prevMS = millis();
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
