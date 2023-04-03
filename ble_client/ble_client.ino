#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "test_complexity/gesture_model_1.h"
// #include "test_complexity/gesture_model_2.h"
// #include "test_complexity/gesture_model_3.h"
// #include "test_complexity/gesture_model_4.h"
// #include "test_complexity/gesture_model_1q.h"
// #include "test_complexity/gesture_model_2q.h"
// #include "test_complexity/gesture_model_3q.h"
// #include "test_complexity/gesture_model_4q.h"

// #include "input_size/gesture_model_s30.h"
// #include "input_size/gesture_model_s20.h"
// #include "input_size/gesture_model_s15q.h"
// #include "input_size/gesture_model_s10q.h"
// #include "input_size/gesture_model_s5.h"
// #include "input_size/gesture_model_s2.h"
// #include "input_size/gesture_model_s1.h"

#include "final/gesture_model_f_s15q.h"


const uint8_t NUM_SAMPLE_MODEL = 15;

namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  constexpr int kTensorArenaSize = 8 * 1024;
  uint8_t tensor_arena[kTensorArenaSize] __attribute__((aligned(16)));
}  // namespace


const char service_uuid[128] = "f30c5d5f-ec5a-4c1d-94c5-46e35d810dc5";
const char web2board_char_uuid[128] = "2f925c9d-0a5b-4217-8e63-2d527c9211c1";
const char board2web_char_uuid[128] = "f8edf338-6bbd-4c3b-bf16-d8d2b6cdaa6e";
const uint8_t device_id = 0;
const char * device_name = device_id ? "DRUM_L" : "DRUM_R";

BLEService service(service_uuid);
// const bool USE_MAGNETOMETER = false;
// without magnetometer
// mx_x 0.123,0.123,0.123, 0.123,0.123,0.123 => 39
// mx_x -4.123,-4.123,-4.123, -2000.1,-2000.1,-2000.1 => 48

// with magnetometer
// mx_x 0.123,0.123,0.123, 0.123,0.123,0.123, 0.123,0.123,0.123 => 57
// mx_x -4.123,-4.123,-4.123, -2000.1,-2000.1,-2000.1, -400.1,-400.1,-400.1 => 69

BLEStringCharacteristic web2boardChar(web2board_char_uuid, BLEWrite | BLENotify, 16);
BLEStringCharacteristic board2webChar(board2web_char_uuid, BLERead | BLENotify, 48);

static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic);


static void onDataCollectMode(const bool trigger, float * aX, float * aY, float * aZ, float * gX, float * gY, float * gZ);
static void onDemoMode(const bool trigger, float * aX, float * aY, float * aZ, float * gX, float * gY, float * gZ);

// variables to be updated by request from webapp
static volatile uint8_t view = 0; // 0: demo, 1: data collection
static volatile uint8_t demoMode = 0; //0: fake inference, 1: real inference

static volatile uint8_t numSampleDataCollection = 15;
static volatile float threshold = 0.16;
static volatile bool canCapture = false;
static volatile uint8_t cooldown = 40;
static volatile uint16_t testResponseTime = 200;

bool isSampling = false;
const uint8_t MAX_POSSIBLE_NUM_SAMPLE = 50;
uint8_t sampleRead = 0;

float aX, aY, aZ, gX, gY, gZ;
bool ledOn = false;
char buf[100];

unsigned long delayCountDown = 0;
#define __delay(duration) ({ \
  delayCountDown = millis();\
  while (millis() - delayCountDown < duration); \
})

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
  service.addCharacteristic(web2boardChar);
  service.addCharacteristic(board2webChar);

  // Add service
  BLE.addService(service);
  web2boardChar.setEventHandler (BLEWritten, onReceiveMsg);

  // start advertising 
  BLE.advertise();
  Serial.print(device_name);
  Serial.println(" is now active, waiting for connections...");

  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

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
  float th;
  if (central) {
    while (central.connected()) {
      
      if (ledOn == 0) {  // turn on the LED to indicate the connection:
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, LOW);
        ledOn = 1;
        Serial.print("Connected to central: "); Serial.println(central.address());
      } 
      
      if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
        IMU.readAcceleration(aX, aY, aZ);
        IMU.readGyroscope(gX, gY, gZ);
        th = (abs(aX / 4.0) + abs(aY / 4.0) + abs(aZ / 4.0) + abs(gX / 2000.0) + abs(gY / 2000.0) + abs(gZ / 2000.0)) / 6.0;

        if (view == 0) onDemoMode(th >= threshold, &aX, &aY, &aZ, &gX, &gY, &gZ);
        else onDataCollectMode(th >= threshold, &aX, &aY, &aZ, &gX, &gY, &gZ);
      }
    }

    if (ledOn == 1) {
      digitalWrite(LEDR, LOW);
      digitalWrite(LEDG, HIGH);
      ledOn = 0;
      Serial.print("Disconnect: "); Serial.println(central.address());
    } 

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
  view = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 1, 1);
  demoMode = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 2, 1);
  canCapture = atoi(valStr) ? true : false;

  strncpy(valStr, ((char *) characteristic.value()) + 3, 2);
  numSampleDataCollection = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 5, 3);
  cooldown = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 8, 3);
  testResponseTime = atoi(valStr);

  strncpy(valStr, ((char *) characteristic.value()) + 11, 4);
  threshold = atof(valStr);

  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);
  __delay(40);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, HIGH);

}

const char* GESTURES[] = {"topl", "twistl", "side", "downl", "twistr", "topr", "downr","unknown"};

int GESTURES_L_I[] = {0, 1, 2, 3, 4, 5, 6, 8};
int GESTURES_R_I[] = {0, 1, 7, 3, 4, 5, 6, 8};

int maxI = -1;
float maxCorr = 0;

unsigned long startT = 0;
unsigned long midT = 0;
unsigned long samplingT;
unsigned long inferenceT;
unsigned long responseT;


static void onDemoMode (const bool trigger, float * aX, float * aY, float * aZ, float * gX, float * gY, float * gZ) {
  if (!isSampling && trigger) {
    if (demoMode == 0) {
      __delay(testResponseTime);
      snprintf(buf, 100, "%dm03", device_id);
      board2webChar.writeValue(buf);
      __delay(cooldown);

    } else {
      isSampling = true;
      sampleRead = 0;
      maxCorr = 0;
      maxI = -1;
      startT = millis();
    }
  }

  if (isSampling) {
    input->data.f[sampleRead * 6] = (*aX + 4.0) / 8.0;
    input->data.f[sampleRead * 6 + 1] = (*aY + 4.0) / 8.0;
    input->data.f[sampleRead * 6 + 2] = (*aZ + 4.0) / 8.0;
    input->data.f[sampleRead * 6 + 3] = (*gX + 2000.0) / 4000.0;
    input->data.f[sampleRead * 6 + 4] = (*gY + 2000.0) / 4000.0;
    input->data.f[sampleRead * 6 + 5] = (*gZ + 2000.0) / 4000.0;

    sampleRead++;

    if (sampleRead == NUM_SAMPLE_MODEL) {
      isSampling = false;
      samplingT = millis() - startT;
      midT = millis();

      // inference
      TfLiteStatus invoke_status = interpreter->Invoke();
      if (invoke_status != kTfLiteOk) {
        MicroPrintf("Invoke failed");
        return;
      }
      inferenceT = millis() - midT;
      
      for (int i = 0; i < sizeof(GESTURES) / sizeof(GESTURES[0]); i++) {
        // Serial.print(String(GESTURES[i]) + ": " + String(output->data.f[i], 2) + " ");

        if (maxCorr < output->data.f[i]) {
          maxI = i;
          maxCorr = output->data.f[i];
        }
      }
      // Serial.println();

      responseT = millis() - startT;
      if (maxCorr > 0.2 && maxI < 7) { // not unknown class, can send signal
        snprintf(buf, 100, "%dm0%d", device_id, device_id ? GESTURES_L_I[maxI] : GESTURES_R_I[maxI]);
        board2webChar.writeValue(buf);
      }

      snprintf(buf, 100, "%s(%.3f), infer:%.3fms sampling:%.3fms response:%.3fms", 
        device_id ? GESTURES[GESTURES_L_I[maxI]] : GESTURES[GESTURES_R_I[maxI]],
        maxCorr, inferenceT, samplingT, responseT
      );
      Serial.println(buf);
      
      
      // cooldown...
      __delay(cooldown);
    }
  }
}

bool isCollecting = false;
float s[MAX_POSSIBLE_NUM_SAMPLE * 6];


static void onDataCollectMode (const bool trigger, float * aX, float * aY, float * aZ, float * gX, float * gY, float * gZ) {
  if (canCapture && !isCollecting && trigger) {
    isCollecting = true;
    sampleRead = 0;
    startT = millis();
  }

  if (isCollecting) {
    s[sampleRead * 6] = (*aX + 4.0) / 8.0;
    s[sampleRead * 6 + 1] = (*aY + 4.0) / 8.0;
    s[sampleRead * 6 + 2] = (*aZ + 4.0) / 8.0;
    s[sampleRead * 6 + 3] = (*gX + 2000.0) / 4000.0;
    s[sampleRead * 6 + 4] = (*gY + 2000.0) / 4000.0;
    s[sampleRead * 6 + 5] = (*gZ + 2000.0) / 4000.0;

    sampleRead++;
    // Serial.println("numSampleDataCollection " + String(numSampleDataCollection));
    if (sampleRead == numSampleDataCollection) {
      isCollecting = false;
      samplingT = millis() - startT;

      startT = millis();
      // Serial.println(String(sampleRead) +" data point(s) collected in " + String(samplingT) + "ms.");

      for (int i=0; i<numSampleDataCollection;i++) {
        // Serial.println("send collected " + String(s[i][0], 3) + " to server");
        snprintf(buf, 100, "%dm1_1%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", device_id, s[i*6], s[i*6+1], s[i*6+2], s[i*6+3], s[i*6+4], s[i*6+5]);
        board2webChar.writeValue(buf);
        __delay(50);
      }

      snprintf(buf, 100, "%dm1_2", device_id);
      board2webChar.writeValue(buf);

      // Serial.println("data collection completed");        
    }
  } else {
    snprintf(buf, 100, "%dm1_0%.3f,%.3f,%.3f,%.1f,%.1f,%.1f", device_id, *aX, *aY, *aZ, *gX, *gY, *gZ);
    board2webChar.writeValue(buf);
    __delay(25);
  }
}

