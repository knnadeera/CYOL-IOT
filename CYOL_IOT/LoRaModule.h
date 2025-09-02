#ifndef LORA_MODULE_H
#define LORA_MODULE_H
#include <Arduino.h>
#include <LoRa.h>

// Forward declaration
class SDCardManager;

void setupLoRa(long frequency, int ss, int rst, int dio0);
void sendLoRaPacket(const String &data);
void lora_receive_data(int ledPin, HardwareSerial &sim7600, SDCardManager *sdCard = nullptr);
int receiveLoRaPacket(String &receivedData);

#endif
