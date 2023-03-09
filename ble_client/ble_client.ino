#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

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
  // Serial.println(BLE.address());
  if (central) {
    Serial.print("Connected to central: "); Serial.println(central.address());

    // turn on the LED to indicate the connection:
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      delay(200);

      // read sensor value
      readValues();

      gesturechar.writeValue(gesture);

      Serial.println("Reading sensors");
      Serial.println(gesture);
      delay(500);
      // delay(500);
    }
  }

  digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED:

  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}

void readValues() {
  gesture = "snare";
}
