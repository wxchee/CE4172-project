#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "gesture_model_q.h"

namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  constexpr int kTensorArenaSize = 8 * 1024;
  uint8_t tensor_arena[kTensorArenaSize] __attribute__((aligned(16)));
}  // namespace


const char service_uuid[128] = "f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5";
const char gesture_char_uuid[128] = "2f925c9d-0a5b-4217-8e63-2d527c9211c1";

const char * device_name = "Drum_L";
// const char * device_name = "Drum_R";

BLEService service(service_uuid);
BLEStringCharacteristic gestureChar(gesture_char_uuid, BLERead | BLEWrite | BLENotify, 47);
static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic);


static void onDataCollectMode();
static void onDemoMode();

void inferSamples();

volatile uint8_t mode = 0; // 0: demo, 1: data collection
volatile bool isSampling = false;
const uint8_t numSample = 20;
uint8_t sampleRead = 0;
float threshold = 0.18;
float aX, aY, aZ, gX, gY, gZ;
// float samples[numSample * 6];
float bf[6];

bool ledOn = false;
// float coolDown = 0;

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

  // Add service
  BLE.addService(service);
  gestureChar.setEventHandler(BLEWritten, onReceiveMsg);

  // start advertising 
  BLE.advertise();
  Serial.print(device_name);
  Serial.println(" is now active, waiting for connections...");

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);

  // ---------------------- Setup TFlite ---------------------------- //
  model = tflite::GetModel(gesture_model_q);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf(
      "Model provided is schema version %d not equal "
      "to supported version %d.",
      model->version(), TFLITE_SCHEMA_VERSION
    );
    return;
  }

  static tflite::AllOpsResolver resolver;

  static tflite::MicroInterpreter static_interpreter(
     model, resolver, tensor_arena, kTensorArenaSize);   // choice 1 AllOpsResolver
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    MicroPrintf("AllocateTensors() failed");
    return;
  }

  // Assign model input and output buffers (tensors) to pointers
  input = interpreter->input(0);
  output = interpreter->output(0);


}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    while (central.connected()) {
      if (ledOn == 0) {  // turn on the LED to indicate the connection:
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, LOW);
        ledOn = 1;
        Serial.print("Connected to central: "); Serial.println(central.address());
      } 
      // data collection mode
      if (mode == 1) { // data collection
        onDataCollectMode();
        continue;
      }

      // demo mode
      onDemoMode();
    }
  }

  if (ledOn == 1) {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    ledOn = 0;
    Serial.print("Disconnect: "); Serial.println(central.address());
  } 

}


static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic) {
  char indexStr[2] = {' ', '\0'};
  strncpy(indexStr, (char *) characteristic.value(), 1);
  mode = atoi(indexStr);
  // Serial.println(String(mode) + ", " + indexStr);
}

const char* GESTURES[] = {"l", "r", "u", "d", "ld", "rd"};
const char* GESTURES_I[] = {"0", "1", "2", "3", "4", "5"};
int maxI = -1;
float maxCorr = 0;
static void onDemoMode () {
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(aX, aY, aZ);
    IMU.readGyroscope(gX, gY, gZ);
    
    // normalise accelerometer and gyroscope abs value to range: [-1, 1]
    bf[0] = aX / 4.0;
    bf[1] = aY / 4.0;
    bf[2] = aZ / 4.0;
    bf[3] = gX / 2000.0;
    bf[4] = gY / 2000.0;
    bf[5] = gZ / 2000.0;

    if (!isSampling && ((abs(bf[0]) + abs(bf[1]) + abs(bf[2]) + abs(bf[3]) + abs(bf[4]) + abs(bf[5])) / 6 >= threshold)) {
      isSampling = true;
      sampleRead = 0;
      maxCorr = 0;
      maxI = -1;
    }

    if (isSampling) {
      input->data.f[sampleRead * 6 + 0] = bf[0];
      input->data.f[sampleRead * 6 + 1] = bf[1];
      input->data.f[sampleRead * 6 + 2] = bf[2];
      input->data.f[sampleRead * 6 + 3] = bf[3];
      input->data.f[sampleRead * 6 + 4] = bf[4];
      input->data.f[sampleRead * 6 + 5] = bf[5];

      sampleRead++;

      if (sampleRead == numSample) {
        isSampling = false;

        // inference
        TfLiteStatus invoke_status = interpreter->Invoke();
        if (invoke_status != kTfLiteOk) {
          MicroPrintf("Invoke failed");
          return;
        }
        for (int i=0; i<6;i++) {
          Serial.print(GESTURES[i]);
          Serial.print(": ");
          Serial.print(output->data.f[i], 2);
          Serial.print(" ");

          if (maxCorr < output->data.f[i]) {
            maxI = i;
            maxCorr = output->data.f[i];
          }
        }

        Serial.println(GESTURES[maxI]);
        


        gestureChar.writeValue(GESTURES_I[maxI]);
        // Serial.println("infer!");
      }
    }

  }
}


static void onDataCollectMode () {
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(aX, aY, aZ);
    IMU.readGyroscope(gX, gY, gZ);
    // normalise accelerometer and gyroscope abs value to range: [0, 1]
    bf[0] = aX / 4.0;
    bf[1] = aY / 4.0;
    bf[2] = aZ / 4.0;
    bf[3] = gX / 2000.0;
    bf[4] = gY / 2000.0;
    bf[5] = gZ / 2000.0;
    gestureChar.writeValue(String(bf[0])+","+String(bf[1])+","+String(bf[2])+","+String(bf[3])+","+String(bf[4])+","+String(bf[5]));
  }
}

void inferSamples() {

}

