#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"

// #include "final_l/gesture_model_f_s15q.h"
#include "final_l/gesture_model_f_s15q.h"
#define DEVICE_ID 1 // 0: RIGHT, 1: Left

#define NUM_SAMPLE_MODEL 15
// mx_x 0.123,0.123,0.123, 0.123,0.123,0.123 => 39
// mx_x -4.123,-4.123,-4.123, -2000.1,-2000.1,-2000.1 => 48
#define READ_LENGTH 16
#define WRITE_LENGTH 48
#define BUF_LENGTH 100

#define __set_LED(r, g, b) ({ \
  digitalWrite(LEDR, r ? LOW : HIGH); \
  digitalWrite(LEDG, g ? LOW : HIGH); \
  digitalWrite(LEDB, b ? LOW : HIGH); \
})

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
// const uint8_t DEVICE_ID = 1;
const char * device_name = DEVICE_ID ? "DRUM_L" : "DRUM_R";

BLEService service(service_uuid);


char buf[BUF_LENGTH];

BLEStringCharacteristic web2boardChar(web2board_char_uuid, BLEWrite | BLENotify, READ_LENGTH);
BLEStringCharacteristic board2webChar(board2web_char_uuid, BLERead | BLENotify, WRITE_LENGTH);

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

bool ledOn = false;


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
  __set_LED(1, 0, 0);

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
  float aX, aY, aZ, gX, gY, gZ;
  if (central) {
    while (central.connected()) {
      
      if (ledOn == 0) {  // turn on the LED to indicate the connection:
        __set_LED(0, 1, 0);
        ledOn = 1;
        Serial.print("Connected to central: "); Serial.println(central.address());
      }

      // snprintf(buf, 100, "view:%d demoM:%d canCap:%d sample:%d cooldown:%d res:%d th:%f", view, demoMode, canCapture, numSampleDataCollection, cooldown, testResponseTime, threshold);
      // Serial.println(buf);
      
      if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
        IMU.readAcceleration(aX, aY, aZ);
        IMU.readGyroscope(gX, gY, gZ);
        th = (abs(aX / 4.0) + abs(aY / 4.0) + abs(aZ / 4.0) + abs(gX / 2000.0) + abs(gY / 2000.0) + abs(gZ / 2000.0)) / 6.0;

        if (view == 0) onDemoMode(th >= threshold, &aX, &aY, &aZ, &gX, &gY, &gZ);
        else onDataCollectMode(th >= threshold, &aX, &aY, &aZ, &gX, &gY, &gZ);
      }
    }

    if (ledOn == 1) {
      __set_LED(1, 0, 0);
      ledOn = 0;
      Serial.print("Disconnect: "); Serial.println(central.address());
    } 

  }

}

static char valStr[6];
static void onReceiveMsg(BLEDevice central, BLECharacteristic characteristic) {
  snprintf(valStr, 2, "%s", (char *) characteristic.value());  // eg. 0
  view = atoi(valStr);
  
  snprintf(valStr, 2, "%s", ((char *) characteristic.value()) + 1); // eg. 0
  demoMode = atoi(valStr); 
  
  snprintf(valStr, 2, "%s", ((char *) characteristic.value()) + 2); // eg. 0
  canCapture = atoi(valStr) ? true : false; 
  
  snprintf(valStr, 3, "%s", ((char *) characteristic.value()) + 3); // eg. 50
  numSampleDataCollection = atoi(valStr);
  
  snprintf(valStr, 4, "%s", ((char *) characteristic.value()) + 5); // eg. 120
  cooldown = atoi(valStr);
  
  snprintf(valStr, 4, "%s", ((char *) characteristic.value()) + 8); // eg. 999
  testResponseTime = atoi(valStr);
  
  snprintf(valStr, 5, "%s", ((char *) characteristic.value()) + 11); // eg. 0.16
  threshold = atof(valStr);

  __set_LED(0, 0, 1);
  __delay(40);
  __set_LED(0, 1, 0);
}

const char* GESTURES[] = {"topl", "twistl", "side", "downl", "twistr", "topr", "downr","unknown"};

int GESTURES_L_I[] = {0, 1, 2, 3};
int GESTURES_R_I[] = {5, 4, 7, 6};

int maxI = -1;
float maxCorr = 0;

unsigned long startT = 0;
unsigned long midT = 0;
unsigned long samplingT;
unsigned long inferenceT;
unsigned long responseT;


static void onDemoMode (const bool trigger, float * aX, float * aY, float * aZ, float * gX, float * gY, float * gZ) {
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
      
      for (int i = 0; i < sizeof(GESTURES_L_I) / sizeof(GESTURES_L_I[0]); i++) {
        // Serial.print(String(GESTURES[i]) + ": " + String(output->data.f[i], 2) + " ");

        if (maxCorr < output->data.f[i]) {
          maxI = i;
          maxCorr = output->data.f[i];
        }
      }
      // Serial.println();

      responseT = millis() - startT;
      if (maxCorr > 0.2) { // met minimum correlation 0.2, can send signal
        snprintf(buf, WRITE_LENGTH, "%dm0%d", DEVICE_ID, DEVICE_ID ? GESTURES_L_I[maxI] : GESTURES_R_I[maxI]);
        board2webChar.writeValue(buf);
      }

      snprintf(buf, BUF_LENGTH, "%s(%.3f), infer:%.lu ms sampling:%lu ms response:%lu ms", 
        DEVICE_ID ? GESTURES[GESTURES_L_I[maxI]] : GESTURES[GESTURES_R_I[maxI]],
        maxCorr, inferenceT, samplingT, responseT
      );
      Serial.println(buf);
      
      
      // cooldown...
      __delay(cooldown);
    }
  } else if (trigger) {
    if (demoMode == 0) {
      __delay(testResponseTime);
      snprintf(buf, WRITE_LENGTH, "%dm03", DEVICE_ID);
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
}

bool isCollecting = false;
float s[MAX_POSSIBLE_NUM_SAMPLE * 6];


static void onDataCollectMode (const bool trigger, float * aX, float * aY, float * aZ, float * gX, float * gY, float * gZ) {
  if (isCollecting) {
    s[sampleRead * 6] = (*aX + 4.0) / 8.0;
    s[sampleRead * 6 + 1] = (*aY + 4.0) / 8.0;
    s[sampleRead * 6 + 2] = (*aZ + 4.0) / 8.0;
    s[sampleRead * 6 + 3] = (*gX + 2000.0) / 4000.0;
    s[sampleRead * 6 + 4] = (*gY + 2000.0) / 4000.0;
    s[sampleRead * 6 + 5] = (*gZ + 2000.0) / 4000.0;

    sampleRead++;
    if (sampleRead == numSampleDataCollection) {
      isCollecting = false;
      samplingT = millis() - startT;

      startT = millis();

      snprintf(buf, BUF_LENGTH, "%d dp collected in %lu ms", sampleRead, samplingT);
      Serial.println(buf);
      // Serial.println(String(sampleRead) +" data point(s) collected in " + String(samplingT) + "ms.");

      for (int i=0; i<numSampleDataCollection;i++) {
        // Serial.println("send collected " + String(s[i][0], 3) + " to server");
        snprintf(buf, WRITE_LENGTH, "%dm1_1%.3f,%.3f,%.3f,%.3f,%.3f,%.3f", DEVICE_ID, s[i*6], s[i*6+1], s[i*6+2], s[i*6+3], s[i*6+4], s[i*6+5]);
        board2webChar.writeValue(buf);
        __delay(50);
      }

      snprintf(buf, WRITE_LENGTH, "%dm1_2", DEVICE_ID);
      board2webChar.writeValue(buf);

      // Serial.println("data collection completed");        
    }
  } else if (canCapture && trigger) {
    isCollecting = true;
    sampleRead = 0;
    startT = millis();
  }

  if (!isCollecting) {
    snprintf(buf, WRITE_LENGTH, "%dm1_0%.3f,%.3f,%.3f,%.1f,%.1f,%.1f", DEVICE_ID, *aX, *aY, *aZ, *gX, *gY, *gZ);
    board2webChar.writeValue(buf);
    __delay(25);
  }
}

