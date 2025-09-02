#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H
#include <Arduino.h>
#include <EEPROM.h>

void saveIDToEEPROM(const String &id, int address, int maxLen);
void loadIDFromEEPROM(String &id, int address, int maxLen);
void clearEEPROM(int address, int len);

#endif
