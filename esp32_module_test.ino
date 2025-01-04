#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVER_NAME "ESP32-BLE"


BLEServer* pServer = NULL;
BLEService* pService = NULL;
BLECharacteristic* dhtCharacteristic = NULL;
BLEAdvertising* pAdvertising = NULL;

bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Device: Connected!");
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Device: Disconnected!");
    BLEDevice::startAdvertising();
  }
};

class CharacteristicCallback : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic* dhtCharacteristic) {
    String value = dhtCharacteristic->getValue().c_str();

    Serial.print(value);
  }
};

void setupBle() {
  Serial.println("BLE initializing...");
  BLEDevice::init(SERVER_NAME);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(SERVICE_UUID);
  dhtCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  dhtCharacteristic->addDescriptor(new BLE2902());
  dhtCharacteristic->setCallbacks(new CharacteristicCallback());

  pService->start();

  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();

  Serial.println("BLE initialized. Waiting for client to connect...");
}

void sendData(String data) {
  Serial.println(data);

  dhtCharacteristic->setValue(data.c_str());
  dhtCharacteristic->notify();
}

void setup() {
  Serial.begin(115200);
  setupBle();
}

void loop() {
  if (Serial.available() > 0) {
    String sendingData = "";

    while (Serial.available() > 0) {
      sendingData += char(Serial.read());
    }

    sendData(incomingData);
    delay(2000);
  }
}
