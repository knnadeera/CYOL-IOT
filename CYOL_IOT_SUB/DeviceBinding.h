#ifndef DEVICE_BINDING_H
#define DEVICE_BINDING_H
#include <Arduino.h>
#include <EEPROM.h>

// External variables that need to be accessed
extern String deviceID;
extern bool binding;
extern bool confirming;
extern volatile unsigned long buttonPressedTime;

// Function declarations
void initDeviceBinding(int deviceAddress);
void binding_function(int pushBtnPin, int ledPin);
void receiveBinding();
void id_confirming();
void saveDeviceID(const String &id);
void loadDeviceIDFromEEPROM();
void clearEEPROMDeviceID(int ledPin);
void sendSensorData(int soilValue, int airValue, int nVal, int pVal, int kVal, int ledPin);

#endif
