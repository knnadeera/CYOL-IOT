#include "BindingManager.h"
#include "../LoRa/LoRaModule.h"
#include "../SIM7600/SIM7600Module.h"

// Static variables for the module
static int MAX_BOUND_IDS = 20;
static int ID_ARRAY_ADDRESS = 0;

void initBindingManager(int maxIds, int arrayAddress)
{
    MAX_BOUND_IDS = maxIds;
    ID_ARRAY_ADDRESS = arrayAddress;
}

void binding_function(int pushBtnPin, int ledPin)
{
    if (digitalRead(pushBtnPin) == HIGH)
    {
        buttonPressedTime++;

        if (buttonPressedTime == 5)
        {
            binding = true;
            // Find an empty slot
            int emptyIndex = -1;
            for (int i = 0; i < MAX_BOUND_IDS; i++)
            {
                if (boundIDs[i] == "")
                {
                    emptyIndex = i;
                    break;
                }
            }

            if (emptyIndex == -1)
            {
                Serial1.println("Error: EEPROM storage full!");
                return;
            }

            String newID = devicePrefix + String(emptyIndex);
            sendLoRaPacket(newID);
        }

        // Button held for 100ms to clear all IDs
        if (buttonPressedTime == 100)
        {
            binding = false;
            clearEEPROMArray(ledPin);
        }
    }
    else
    {
        buttonPressedTime = 0;
    }
}

void lora_receive_data(int ledPin, HardwareSerial &sim7600)
{
    String receivedData = "";
    int packetSize = receiveLoRaPacket(receivedData);
    if (packetSize)
    {
        loadIDsFromEEPROM();
        digitalWrite(ledPin, HIGH);

        // Parse the received data
        int splitPos = receivedData.indexOf(":");
        int soilPos = receivedData.indexOf("soil=");
        int airPos = receivedData.indexOf(",air=");
        int nValuePos = receivedData.indexOf(",nValue=");
        int pValuePos = receivedData.indexOf(",pValue=");
        int kValuePos = receivedData.indexOf(",kValue=");

        if (splitPos == -1 || soilPos == -1 || airPos == -1 || nValuePos == -1 || pValuePos == -1 || kValuePos == -1)
        {
            Serial1.println("Invalid LoRa data format");
            digitalWrite(ledPin, LOW);
            return;
        }

        String id = receivedData.substring(0, splitPos);
        String soilData = receivedData.substring(soilPos + 5, airPos);
        String airData = receivedData.substring(airPos + 5, nValuePos);
        String nValueData = receivedData.substring(nValuePos + 8, pValuePos);
        String pValueData = receivedData.substring(pValuePos + 8, kValuePos);
        String kValueData = receivedData.substring(kValuePos + 8);

        // Check if the received ID is valid
        if (isValidID(id))
        {
            Serial1.println("Received ID: " + id);
            Serial1.println("Soil data: " + soilData);
            Serial1.println("Air data: " + airData);
            Serial1.println("N data: " + nValueData);
            Serial1.println("P data: " + pValueData);
            Serial1.println("K data: " + kValueData);

            // Create JSON payload
            String jsonPayload = "{";
            jsonPayload += "\"sensorID\": \"" + id + "\",";
            jsonPayload += "\"soilData\": \"" + soilData + "\",";
            jsonPayload += "\"airData\": \"" + airData + "\",";
            jsonPayload += "\"nData\": \"" + nValueData + "\",";
            jsonPayload += "\"pData\": \"" + pValueData + "\",";
            jsonPayload += "\"kData\": \"" + kValueData + "\"";
            jsonPayload += "}";

            // Send data to API
            sendPostRequest(sim7600, backendURL, jsonPayload);
        }
        else
        {
            Serial1.println("Unauthorized ID. Ignoring...");
        }

        digitalWrite(ledPin, LOW);
    }
}

void lora_confirm_bind()
{
    String receivedID = "";
    int packetSize = receiveLoRaPacket(receivedID);
    if (packetSize)
    {
        saveID(receivedID);
        sendLoRaPacket(receivedID);
        binding = false;
    }
}

bool isValidID(String id)
{
    for (int i = 0; i < MAX_BOUND_IDS; i++)
    {
        if (boundIDs[i] == id)
        {
            return true;
        }
    }
    return false;
}

void loadIDsFromEEPROM()
{
    int address = 0;

    for (int i = 0; i < MAX_BOUND_IDS; i++)
    {
        char idBuffer[20];
        int j = 0;

        while (j < 19)
        {
            char c = EEPROM.read(address + j);
            if (c == '\0')
                break;
            idBuffer[j] = c;
            j++;
        }
        idBuffer[j] = '\0';

        boundIDs[i] = String(idBuffer);
        address += (j + 1);
    }
}

void saveID(String newID)
{
    int address = ID_ARRAY_ADDRESS;

    // Check if ID already exists
    for (int i = 0; i < MAX_BOUND_IDS; i++)
    {
        if (boundIDs[i] == newID)
        {
            Serial1.println("ID already exists.");
            return;
        }
    }

    // Find an empty slot
    int emptyIndex = -1;
    for (int i = 0; i < MAX_BOUND_IDS; i++)
    {
        if (boundIDs[i] == "")
        {
            emptyIndex = i;
            break;
        }
    }

    if (emptyIndex == -1)
    {
        Serial1.println("Error: EEPROM storage full!");
        return;
    }

    boundIDs[emptyIndex] = newID;

    for (int i = 0; i < MAX_BOUND_IDS; i++)
    {
        int len = boundIDs[i].length();
        for (int j = 0; j < len; j++)
        {
            EEPROM.write(address + j, boundIDs[i][j]);
        }
        EEPROM.write(address + len, '\0');
        address += (len + 1);
    }
    loadIDsFromEEPROM();
}

void clearEEPROMArray(int ledPin)
{
    digitalWrite(ledPin, HIGH);
    for (int i = 0; i < MAX_BOUND_IDS * 10; i++)
    {
        EEPROM.write(ID_ARRAY_ADDRESS + i, 0);
    }
    digitalWrite(ledPin, LOW);
}
