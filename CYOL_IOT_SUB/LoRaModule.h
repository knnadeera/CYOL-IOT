#ifndef LORA_MODULE_H
#define LORA_MODULE_H
#include <Arduino.h>
#include <LoRa.h>

void setupLoRa(long frequency, int ss, int rst, int dio0);
void sendLoRaPacket(const String &data);
int receiveLoRaPacket(String &receivedData);

#endif
