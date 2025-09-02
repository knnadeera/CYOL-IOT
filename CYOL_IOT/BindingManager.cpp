#include "BindingManager.h"
#include "LoRaModule.h"
#include "SIM7600Module.h"

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
