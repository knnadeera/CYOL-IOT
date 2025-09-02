#include "LoRaModule.h"

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
