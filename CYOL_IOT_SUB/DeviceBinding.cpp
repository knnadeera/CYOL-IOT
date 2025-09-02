#include "DeviceBinding.h"
#include "../LoRa/LoRaModule.h"

// Static variables for the module
static int DEVICE_ID_ADDRESS = 0;

void initDeviceBinding(int deviceAddress)
{
    DEVICE_ID_ADDRESS = deviceAddress;
}

void binding_function(int pushBtnPin, int ledPin)
{
    if (digitalRead(pushBtnPin) == HIGH)
    {
        buttonPressedTime++;

        if (buttonPressedTime == 5)
        {
            binding = true;
        }

        // Clear EEPROM device ID after 100ms
        if (buttonPressedTime == 100)
        {
            binding = false;
            clearEEPROMDeviceID(ledPin);
        }
    }
    else
    {
        buttonPressedTime = 0;
    }
}

void receiveBinding()
{
    Serial1.println("Receiving");
    String receivedID = "";
    int packetSize = receiveLoRaPacket(receivedID);
    if (packetSize)
    {
        Serial1.println("Received binding ID: " + receivedID);
        saveDeviceID(receivedID);
        delay(3000);
        confirming = true;
        Serial1.println("Binding complete!");
    }
}

void id_confirming()
{
    sendLoRaPacket(deviceID);

    String receivedID = "";
    int packetSize = receiveLoRaPacket(receivedID);
    if (packetSize)
    {
        confirming = false;
        binding = false;
    }
}

void saveDeviceID(const String &id)
{
    clearEEPROMDeviceID(PC13); // Use default LED pin
    deviceID = id;

    int address = DEVICE_ID_ADDRESS;
    // Write each character of the string to EEPROM
    for (int i = 0; i < id.length(); i++)
    {
        EEPROM.write(address + i, id[i]);
    }
    EEPROM.write(address + id.length(), '\0');

    loadDeviceIDFromEEPROM();
    Serial1.println("ID saved: " + id);
}

void loadDeviceIDFromEEPROM()
{
    int address = DEVICE_ID_ADDRESS;
    char idBuffer[20];
    int i = 0;

    while (true)
    {
        char c = EEPROM.read(address + i);
        if (c == '\0' || i >= sizeof(idBuffer) - 1)
        {
            break;
        }
        idBuffer[i] = c;
        i++;
    }
    idBuffer[i] = '\0';

    deviceID = String(idBuffer);
    Serial1.println("ID loaded: " + deviceID);
}

void clearEEPROMDeviceID(int ledPin)
{
    digitalWrite(ledPin, HIGH);
    int address = DEVICE_ID_ADDRESS;

    for (int i = 0; i < 20; i++)
    {
        EEPROM.write(address + i, 0xFF);
    }

    loadDeviceIDFromEEPROM();
    Serial1.println("EEPROM array cleared.");
    digitalWrite(ledPin, LOW);
}

void sendSensorData(int soilValue, int airValue, int nVal, int pVal, int kVal, int ledPin)
{
    digitalWrite(ledPin, HIGH);

    if (deviceID == "")
    {
        Serial1.println("No ID assigned! Skipping transmission.");
        digitalWrite(ledPin, LOW);
        return;
    }

    // Create the data packet
    String dataPacket = deviceID + ":" + "soil=" + String(soilValue) +
                        ",air=" + String(airValue) + ",nValue=" + String(nVal) +
                        ",pValue=" + String(pVal) + ",kValue=" + String(kVal);

    sendLoRaPacket(dataPacket);
    digitalWrite(ledPin, LOW);
    Serial1.println("Data sent: " + dataPacket);
}
