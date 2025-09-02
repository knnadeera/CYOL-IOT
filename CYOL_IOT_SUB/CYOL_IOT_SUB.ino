#include <string.h>
#include <stdio.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <stm32f1xx_hal.h>
#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>
#include "modules/LoRa/LoRaModule.h"
#include "modules/SoilSensor/SoilSensor.h"
#include "modules/EEPROM/EEPROMManager.h"
#include "modules/LEDControl/LEDControl.h"
#include "modules/DeviceBinding/DeviceBinding.h"

// Define constants
#define LORA_FREQ 433E6            // LoRa frequency
#define SLEEP_TIMEOUT 15 300000    // 30 minutes
#define SOIL_SENSOR_INTERVAL 60000 // Send data every 10 seconds
#define DEVICE_ID_ADDRESS 0

// Pin Definitions
#define DO_PIN PB3       // Pin for soil sensor digital
#define AO_PIN PA1       // Pin for soil sensor annalog
#define PUSH_BTN_PIN PB5 // Pin for button
#define LED_PIN PC13     // Pin for LED
#define SS PA4           // Pin for LoRa module
#define RST PB1          // Pin for LoRa module
#define DIO0 PB0         // Pin for LoRa module

// Global variables
String deviceID = "";
bool binding = false;
bool receiving = false;
bool confirming = false;
int soilMoistureValue = 0;
int airMoistureValue = 10;
int nValue = 20;
int pValue = 30;
int kValue = 40;
unsigned long lastSoilSensorReadTime = 0;
volatile unsigned long buttonPressedTime = 0;

// Function prototypes
void wakeUpISR();

void setup()
{
  Serial1.begin(115200);
  delay(3000); // Allow initialization

  pinMode(LED_PIN, OUTPUT);
  pinMode(PUSH_BTN_PIN, INPUT);
  pinMode(DO_PIN, INPUT);
  pinMode(AO_PIN, INPUT_ANALOG);
  digitalWrite(LED_PIN, LOW);

  // Initialize LoRa module (using new module)
  setupLoRa(LORA_FREQ, SS, RST, DIO0);

  // Initialize device binding
  initDeviceBinding(DEVICE_ID_ADDRESS);

  // Load stored ID
  loadDeviceIDFromEEPROM();

  // LED bootup pattern
  runLEDPattern(LED_PIN);
  delay(2000);

  // Configure button interrupt
  attachInterrupt(digitalPinToInterrupt(PUSH_BTN_PIN), wakeUpISR, FALLING);
}

void loop()
{
  if (digitalRead(PUSH_BTN_PIN) == HIGH)
  {
    delay(100);
  }

  if (binding)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink LED in binding mode
    if (!confirming)
    {
      receiveBinding();
    }
    else
    {
      id_confirming();
    }
  }
  else
  {
    if (millis() - lastSoilSensorReadTime >= SOIL_SENSOR_INTERVAL)
    {
      lastSoilSensorReadTime = millis();
      int sensorValue = readSoilMoisture(AO_PIN, DO_PIN);
      if (sensorValue != -1)
      {
        soilMoistureValue = sensorValue;
        sendSensorData(soilMoistureValue, airMoistureValue, nValue, pValue, kValue, LED_PIN);
      }
    }
  }
  binding_function(PUSH_BTN_PIN, LED_PIN);
}

// ...LoRa, SoilSensor, LED and DeviceBinding functions now in modules...

// Wakeup function
void wakeUpISR()
{
  receiving = true;
}
