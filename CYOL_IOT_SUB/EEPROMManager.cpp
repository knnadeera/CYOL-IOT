#include "EEPROMManager.h"

void saveIDToEEPROM(const String &id, int address, int maxLen)
{
    int len = id.length();
    for (int i = 0; i < len && i < maxLen - 1; i++)
    {
        EEPROM.write(address + i, id[i]);
    }
    EEPROM.write(address + len, '\0');
}

void loadIDFromEEPROM(String &id, int address, int maxLen)
{
    char buffer[32];
    int i = 0;
    for (; i < maxLen - 1; i++)
    {
        char c = EEPROM.read(address + i);
        if (c == '\0')
            break;
        buffer[i] = c;
    }
    buffer[i] = '\0';
    id = String(buffer);
}

void clearEEPROM(int address, int len)
{
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(address + i, 0xFF);
    }
}
