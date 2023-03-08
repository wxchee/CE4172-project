#include <ArduinoBLE.h>

// Set the service and characteristic:
BLEService customService("180A");

// BLE Characteristics
// Syntax: BLE<DATATYPE>Characteristic <NAME>(<UUID>, <PROPERTIES>, <DATA LENGTH>)
// eg. BLEByteCharacteristic switchCharacteristic("2A57", BLERead | BLEWrite);
BLEStringCharacteristic ble_gesture("2A56", BLERead | BLENotify, 20);

int r,g,b;
String gesture;
char * device_name = "Nano33BLE (Left)";
void readValues();

void setup() {

  Serial.begin(9600);

  if (!BLE.begin()) {
    Serial.println("BLE failed to initialise");
    delay(500);
    while(1);
  }

  // Set advertised name and service:
  BLE.setLocalName(device_name);
  BLE.setAdvertisedService(customService);
  
  customService.addCharacteristic(ble_gesture);

  // Add service
  BLE.addService(customService);

  // start advertising 
  BLE.advertise();
  Serial.print(device_name);
  Serial.print("(");
  Serial.print(")");
  Serial.println(" is now active, waiting for connections...");
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice central = BLE.central();
  // Serial.println(BLE.address());
  if (central) {
    Serial.print("Connected to central: "); Serial.println(central.address());

    while (central.connected()) {
      delay(200);

      // read sensor value
      readValues();

      ble_gesture.writeValue(gesture);

      Serial.println("Reading sensors");
      Serial.println(gesture);
      delay(500);
      // delay(500);
    }
  }

  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
}

void readValues() {
  gesture = "snare";
}
