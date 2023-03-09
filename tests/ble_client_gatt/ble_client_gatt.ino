#include <ArduinoBLE.h>
#include <BLEDevice.h>
// Replace this with the address of your server
const char* server_address = "0c:7a:15:ac:75:78";
// Replace this with the UUID of your custom GATT service and characteristic
const char* service_uuid = "180A";
const char* characteristic_uuid = "2A56";

BLEClient client;
BLECharacteristic data_characteristic;

void setup() {
  Serial.begin(9600);

  // Initialize the BLE library
  if (!BLE.begin()) {
    Serial.println("Failed to initialize BLE");
    while (1);
  }

  // Connect to the server
  Serial.print("Connecting to server...");
  client.connect(server_address);
  Serial.println("connected");

  // Discover the service and characteristic
  BLERemoteService service = client.getService(service_uuid);
  if (!service) {
    Serial.println("Failed to find service");
    client.disconnect();
    while (1);
  }
  data_characteristic = service.getCharacteristic(characteristic_uuid);
  if (!data_characteristic) {
    Serial.println("Failed to find characteristic");
    client.disconnect();
    while (1);
  }
}

void loop() {
  // Send data to the server
  String data = "Hello, server!";
  data_characteristic.writeValue(data.c_str(), data.length());
  Serial.println("Sent data: " + data);

  delay(1000);
}