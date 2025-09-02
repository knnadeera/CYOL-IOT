#include <SPI.h>
#include <LoRa.h>
#include <EEPROM.h>
#include <SdFat.h>
#include <ArduinoJson.h>
#include "LoRaModule.h"
#include "SoilSensor.h"
#include "LEDControl.h"
#include "EEPROMManager.h"
#include "SIM7600Module.h"
#include "BindingManager.h"
#include "SDCardManager.h"

// Define constants
#define LORA_FREQ 433E6      // LoRa frequency
#define SLEEP_TIMEOUT 300000 // 30 minutes
#define MAX_BOUND_IDS 10
#define SOIL_SENSOR_INTERVAL 60000 // Interval for reading soil sensor (10s)
#define ID_ARRAY_ADDRESS 0
#define EEPROM_I2C_ADDR 0x50 // EEPROM I2C Address (Check your EEPROM)
#define EEPROM_SIZE 32768    // 32KB (AT24C256)

// Pin Definitions
// FTDI Pin Definitions
#define FTDI_RX_PIN PA9  // Pin for FTDI RX
#define FTDI_TX_PIN PA10 // Pin for FTDI TX
// Soil Sensor Pin Definitions
#define DO_PIN PB3 // Pin for soil sensor digital
#define AO_PIN PA1 // Pin for soil sensor annalog
// Button and LED Pin Definitions
#define PUSH_BTN_PIN PB5 // Pin for button
#define LED_PIN PC13     // Pin for LED
// LoRa Pin Definitions
#define NSS PA4  // Pin for LoRa module
#define RST PB1  // Pin for LoRa module
#define DIO0 PB0 // Pin for LoRa module
// SIM7600 Pin Definitions
#define SIM7600_RX PA3 // Connect SIM7600 RX to this pin
#define SIM7600_TX PA2 // Connect SIM7600 TX to this pin

// Shared SPI Pins for LoRa and SD Card
#define SPI_SCK_PIN PA5
#define SPI_MISO_PIN PA6
#define SPI_MOSI_PIN PA7

// Unique CS pins for each device
#define SD_CS_PIN PB4
#define LORA_CS_PIN PA4
SDCardManager sdCard(SD_CS_PIN);

// Global variables
const char *devicePrefix = "MAIN_"; // Prefix for identification
String boundIDs[MAX_BOUND_IDS];     // Storage for bound IDs
bool binding = false;
bool receiving = false;
int soilMoistureValue = 0;
unsigned long lastActivityTime = 0;
unsigned long lastSoilSensorReadTime = 0;
volatile unsigned long buttonPressedTime = 0;
unsigned long lastSDStatusTime = 0;
#define SD_STATUS_INTERVAL 300000 // Log SD status every 5 minutes
const char *timeZone = "05_30";
const char *backendURL = "http://52.73.93.193:3001/database-script/data-log-create";

// Create a HardwareSerial object for SIM7600 communication
HardwareSerial sim7600(SIM7600_RX, SIM7600_TX);

// ...existing code...
void wakeUpISR();

void setup()
{
  Serial1.begin(115200);
  sim7600.begin(115200); // Re-enabled SIM7600 communication

  // Configure pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(PUSH_BTN_PIN, INPUT);
  pinMode(DO_PIN, INPUT);
  pinMode(AO_PIN, INPUT_ANALOG);
  digitalWrite(LED_PIN, LOW);

  // Initialize LoRa module
  setupLoRa(LORA_FREQ, NSS, RST, DIO0);

  Serial1.println("=== SD Card Init Complete ===");

  // Initialize binding manager
  initBindingManager(MAX_BOUND_IDS, ID_ARRAY_ADDRESS);

  // Load stored IDs
  loadIDsFromEEPROM();

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

  // Binding functionality disabled for SD card testing
  if (binding)
  {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink LED during binding
    lora_confirm_bind();
  }
  else
  {
    // LoRa functionality temporarily disabled for SD card testing
    lora_receive_data(LED_PIN, sim7600, sdCard.isInitialized() ? &sdCard : nullptr);
    Serial1.println("LoRa disabled - testing SD card with exclusive SPI access");

    // Read soil sensor at intervals
    if (millis() - lastSoilSensorReadTime >= SOIL_SENSOR_INTERVAL)
    {
      lastSoilSensorReadTime = millis();
      int sensorValue = readSoilMoisture(AO_PIN, DO_PIN);
      if (sensorValue != -1)
      {
        soilMoistureValue = sensorValue;
        Serial1.print("Soil: ");
        Serial1.println(soilMoistureValue);

        // Safe SD card logging with checks
        if (sdCard.isInitialized())
        {
          String logData = "Local Soil Reading: " + String(soilMoistureValue);
          if (sdCard.writeDataWithTimestamp(logData))
          {
            Serial1.println("Logged to SD card successfully");
          }
          else
          {
            Serial1.println("SD card write failed - continuing anyway");
          }
        }
      }
    }
  }
  binding_function(PUSH_BTN_PIN, LED_PIN);

  // Re-enabled SIM7600 functionality
  while (sim7600.available())
  {
    String response = sim7600.readString();
    Serial1.println(response); // Print network time
  }

  // Heartbeat LED - blink every 5 seconds to show system is alive
  static unsigned long lastHeartbeat = 0;
  if (millis() - lastHeartbeat >= 5000)
  {
    lastHeartbeat = millis();
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
  }
}

// Wakeup function
void wakeUpISR()
{
  receiving = true;
}