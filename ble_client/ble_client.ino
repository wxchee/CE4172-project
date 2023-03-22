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

const uint8_t device_id = 1;
const char * device_name = device_id ? "DRUM_L" : "DRUM_R";
// const char * device_name = "Drum_R";

BLEService service(service_uuid);
BLEStringCharacteristic gestureChar(gesture_char_uuid, BLERead | BLEWrite | BLENotify, 46);
static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic);


static void onDataCollectMode();
static void onDemoMode();

// variables to be updated by request from webapp
static volatile uint8_t mode = 0; // 0: demo, 1: data collection
static volatile uint8_t numSample = 15;
static volatile float threshold = 0.16;
static volatile bool canCapture = false;
static volatile uint8_t cooldown = 40;

bool isSampling = false;
const uint8_t MAX_POSSIBLE_NUM_SAMPLE = 50;
uint8_t sampleRead = 0;

float aX, aY, aZ, gX, gY, gZ;
float bf[6];

bool ledOn = false;

// float coolDown = 0;
float dt = 0;
float prevMs = 0;

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
  gestureChar.setEventHandler (BLEWritten, onReceiveMsg);

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
      
      if (mode == 1) onDataCollectMode();
      else onDemoMode();

    }
  }

  if (ledOn == 1) {
    digitalWrite(LEDR, LOW);
    digitalWrite(LEDG, HIGH);
    ledOn = 0;
    Serial.print("Disconnect: "); Serial.println(central.address());
  } 

}

static char valStr[6];
static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic) {
  valStr[0] = ' ';
  valStr[1] = ' ';
  valStr[2] = ' ';
  valStr[3] = ' ';
  valStr[4] = ' ';
  valStr[5] = ' ';

  strncpy(valStr, ((char *) characteristic.value()), 1);
  mode = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 1, 1);
  canCapture = atoi(valStr) ? true : false;

  strncpy(valStr, ((char *) characteristic.value()) + 2, 2);
  numSample = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 4, 2);
  cooldown = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 6, 5);
  threshold = atof(valStr);
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
      input->data.f[sampleRead * 6 + 0] = (aX + 4.0) / 8.0;
      input->data.f[sampleRead * 6 + 1] = (aY + 4.0) / 8.0;
      input->data.f[sampleRead * 6 + 2] = (aZ + 4.0) / 8.0;
      input->data.f[sampleRead * 6 + 3] = (gX + 2000.0) / 4000.0;
      input->data.f[sampleRead * 6 + 4] = (gY + 2000.0) / 4000.0;
      input->data.f[sampleRead * 6 + 5] = (gZ + 2000.0) / 4000.0;

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
          // Serial.print(String(GESTURES[i]) + ": " + String(output->data.f[i], 2) + " ");

          if (maxCorr < output->data.f[i]) {
            maxI = i;
            maxCorr = output->data.f[i];
          }
        }
        gestureChar.writeValue(String(device_id) + "m0" + String(GESTURES_I[maxI]));
        Serial.println(String(GESTURES[maxI]));

        // cooldown...
        delay(cooldown);
      }
    } else {
      gestureChar.writeValue(String(device_id) + "m1_0" + String(aX,3)+","+String(aY,3)+","+String(aZ,3)+","+String(gX, 1)+","+String(gY, 1)+","+String(gZ, 1));
      delay(25);
    }

  }
}

bool isCollecting = false;
float s[MAX_POSSIBLE_NUM_SAMPLE][6];

static void onDataCollectMode () {
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(aX, aY, aZ);
    IMU.readGyroscope(gX, gY, gZ);

    bf[0] = aX / 4.0;
    bf[1] = aY / 4.0;
    bf[2] = aZ / 4.0;
    bf[3] = gX / 2000.0;
    bf[4] = gY / 2000.0;
    bf[5] = gZ / 2000.0;

    if (canCapture && !isCollecting && ((abs(bf[0]) + abs(bf[1]) + abs(bf[2]) + abs(bf[3]) + abs(bf[4]) + abs(bf[5])) / 6 >= threshold)) {
      isCollecting = true;
      sampleRead = 0;
      maxCorr = 0;
      maxI = -1;
      // gestureChar.writeValue("m1_0");
      // Serial.println("start capture.");
    }

    if (isCollecting) {
      s[sampleRead][0] = (aX + 4.0) / 8.0;
      s[sampleRead][1] = (aY + 4.0) / 8.0;
      s[sampleRead][2] = (aZ + 4.0) / 8.0;
      s[sampleRead][3] = (gX + 2000.0) / 4000.0;
      s[sampleRead][4] = (gY + 2000.0) / 4000.0;
      s[sampleRead][5] = (gZ + 2000.0) / 4000.0;
      sampleRead++;
      // Serial.println("numSample " + String(numSample));
      if (sampleRead == numSample) {
        isCollecting = false;
        for (int i=0; i<numSample;i++) {
          // Serial.println("send collected " + String(s[i][0], 3) + " to server");
          gestureChar.writeValue(String(device_id) + "m1_1" + String(s[i][0], 3)+","+String(s[i][1], 3)+","+String(s[i][2], 3)+","+String(s[i][3], 3)+","+String(s[i][4], 3)+","+String(s[i][5], 3));
          delay(50);
        }
        gestureChar.writeValue(String(device_id) + "m1_2");

        Serial.println("data collection completed in " + String(millis() - dt));        
      }
    } else {
      gestureChar.writeValue(String(device_id) + "m1_0" + String(aX,3)+","+String(aY,3)+","+String(aZ,3)+","+String(gX, 1)+","+String(gY, 1)+","+String(gZ, 1));
      delay(25);
    }
  }
}


