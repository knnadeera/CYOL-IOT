#ifndef SD_CARD_MANAGER_H
#define SD_CARD_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

class SDCardManager
{
private:
    int csPin;
    bool initialized;
    String logFileName;
    SdFat sd;

public:
    SDCardManager(int chipSelectPin);
    bool begin();
    bool isInitialized();
    bool writeData(String data);
    bool writeDataWithTimestamp(String data, String timestamp = "");
    bool appendToFile(String fileName, String data);
    bool createFile(String fileName);
    bool deleteFile(String fileName);
    bool fileExists(String fileName);
    long getFileSize(String fileName);
    void setLogFileName(String fileName);
    String getLogFileName();
    void listFiles();
    void logSDCardCapacity();
    void testSPIConnection();
};

#endif