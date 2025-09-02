#ifndef BINDING_MANAGER_H
#define BINDING_MANAGER_H
#include <Arduino.h>
#include <EEPROM.h>

// External variables that need to be accessed
extern String devicePrefix;
extern String boundIDs[];
extern bool binding;
extern volatile unsigned long buttonPressedTime;
extern String backendURL;

// Function declarations
void initBindingManager(int maxIds, int arrayAddress);
void binding_function(int pushBtnPin, int ledPin);
void lora_receive_data(int ledPin, HardwareSerial &sim7600);
void lora_confirm_bind();
bool isValidID(String id);
void loadIDsFromEEPROM();
void saveID(String newID);
void clearEEPROMArray(int ledPin);

#endif
