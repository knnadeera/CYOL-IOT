#ifndef SIM7600_MODULE_H
#define SIM7600_MODULE_H
#include <Arduino.h>

bool sendCommandCheck(HardwareSerial &sim7600, String command, String expectedResponse, int timeout = 5000);
bool checkModuleReady(HardwareSerial &sim7600);
void sendPostRequest(HardwareSerial &sim7600, const String &backendURL, const String &jsonData);

#endif
