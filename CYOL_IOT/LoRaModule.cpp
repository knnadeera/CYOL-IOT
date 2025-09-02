#include "LoRaModule.h"
#include "BindingManager.h"
#include "SIM7600Module.h"
// #include "SDCardManager.h"  // Temporarily disabled

void setupLoRa(long frequency, int ss, int rst, int dio0)
{
    LoRa.setPins(ss, rst, dio0);
    if (!LoRa.begin(frequency))
    {
        while (1)
            ;
    }
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.setTxPower(20);
}

void sendLoRaPacket(const String &data)
{
    LoRa.beginPacket();
    LoRa.print(data);
    LoRa.endPacket();
}

int receiveLoRaPacket(String &receivedData)
{
    int packetSize = LoRa.parsePacket();
    receivedData = "";
    if (packetSize)
    {
        while (LoRa.available())
        {
            receivedData += (char)LoRa.read();
        }
    }
    return packetSize;
}

void lora_receive_data(int ledPin, HardwareSerial &sim7600, SDCardManager *sdCard)
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
            // Reduced debug output to save flash
            Serial1.println("Valid ID received");

            // SD card logging temporarily disabled
            /*
            if (sdCard != nullptr && sdCard->isInitialized())
            {
                String logData = "LoRa RX | ID:" + id + " | Soil:" + soilData +
                                 " | Air:" + airData + " | N:" + nValueData +
                                 " | P:" + pValueData + " | K:" + kValueData;
                sdCard->writeDataWithTimestamp(logData);
            }
            */

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

            // SD card logging temporarily disabled
            /*
            if (sdCard != nullptr && sdCard->isInitialized())
            {
                sdCard->writeDataWithTimestamp("API TX | " + jsonPayload);
            }
            */
        }
        else
        {
            Serial1.println("Unauthorized ID");

            // SD card logging temporarily disabled
            /*
            if (sdCard != nullptr && sdCard->isInitialized())
            {
                sdCard->writeDataWithTimestamp("UNAUTHORIZED | ID:" + id + " | " + receivedData);
            }
            */
        }

        digitalWrite(ledPin, LOW);
    }
}
