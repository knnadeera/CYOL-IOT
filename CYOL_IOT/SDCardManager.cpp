#include "SDCardManager.h"

SDCardManager::SDCardManager(int chipSelectPin) : csPin(chipSelectPin), initialized(false), logFileName("data.txt") {}

bool SDCardManager::begin()
{
    Serial.print("Initializing SD card on pin ");
    Serial.print(csPin);
    Serial.print("... ");

    // Add SPI diagnostic information
    Serial.println();
    Serial.println("=== SD Card SPI Diagnostic ===");
    Serial.print("CS Pin (Chip Select): ");
    Serial.println(csPin);
    Serial.println("Expected SPI Connections:");
    Serial.println("  SD CS   -> PB4 (pin " + String(csPin) + ")");
    Serial.println("  SD SCK  -> PA5  (EXCLUSIVE - LoRa disabled)");
    Serial.println("  SD MOSI -> PA7  (EXCLUSIVE - LoRa disabled)");
    Serial.println("  SD MISO -> PA6  (EXCLUSIVE - LoRa disabled)");
    Serial.println("  SD VCC  -> 3.3V");
    Serial.println("  SD GND  -> GND");
    Serial.println("  LoRa: DISABLED for testing");
    Serial.println("===============================");

    // Ensure SD CS is properly configured
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH); // Deselect SD initially
    Serial.println("SD card CS configured (PB4 set HIGH)");

    Serial.print("Attempting SD card initialization with EXCLUSIVE SPI access... ");
    if (!sd.begin(csPin))
    {
        Serial.println("FAILED!");
        Serial.println("=== SD Card Troubleshooting (Shared SPI) ===");
        Serial.println("Possible issues:");
        Serial.println("1. SD card not inserted properly");
        Serial.println("2. Loose SPI wire connections");
        Serial.println("3. SD card not FAT32 formatted");
        Serial.println("4. Power supply issues (needs stable 3.3V)");
        Serial.println("5. Defective SD card - try another one");
        Serial.println("6. SPI conflict - LoRa interfering with SD");
        Serial.println("7. Both devices need separate CS pins");
        Serial.println("   LoRa CS=PA4, SD CS=PB4 (verified above)");
        Serial.println("===============================");
        initialized = false;
        return false;
    }

    Serial.println("SUCCESS!");
    initialized = true;

    // Log SD card capacity in MB
    logSDCardCapacity();

    return true;
}
void SDCardManager::logSDCardCapacity()
{
    if (!initialized)
    {
        Serial.println("SD card not initialized - cannot read capacity");
        return;
    }

    // Get card size in sectors (512 bytes each)
    uint32_t cardSectors = sd.card()->sectorCount();

    // Calculate capacity in MB (1 MB = 1024 * 1024 bytes = 2048 sectors of 512 bytes)
    float capacityMB = (float)cardSectors * 512.0 / (1024.0 * 1024.0);

    Serial.print("SD Card Capacity: ");
    Serial.print(capacityMB, 2); // 2 decimal places
    Serial.println(" MB");

    // Also get free space if possible
    uint32_t freeKB = 0;
    if (sd.freeClusterCount() > 0)
    {
        freeKB = sd.freeClusterCount() * sd.sectorsPerCluster() * 512 / 1024;
        float freeMB = freeKB / 1024.0;

        Serial.print("SD Card Free Space: ");
        Serial.print(freeMB, 2);
        Serial.println(" MB");

        float usedMB = capacityMB - freeMB;
        float usagePercent = (usedMB / capacityMB) * 100.0;

        Serial.print("SD Card Usage: ");
        Serial.print(usedMB, 2);
        Serial.print(" MB (");
        Serial.print(usagePercent, 1);
        Serial.println("%)");
    }
}

bool SDCardManager::isInitialized()
{
    return initialized;
}

bool SDCardManager::writeData(String data)
{
    if (!initialized)
    {
        Serial.println("SD card not initialized");
        return false;
    }

    return appendToFile(logFileName, data);
}

bool SDCardManager::writeDataWithTimestamp(String data, String timestamp)
{
    if (!initialized)
    {
        Serial.println("SD card not initialized");
        return false;
    }

    String timestampedData = "";

    if (timestamp == "")
    {
        // Use millis() as simple timestamp if no timestamp provided
        timestampedData = String(millis()) + ": " + data;
    }
    else
    {
        timestampedData = timestamp + ": " + data;
    }

    return appendToFile(logFileName, timestampedData);
}

bool SDCardManager::appendToFile(String fileName, String data)
{
    if (!initialized)
    {
        Serial.println("SD card not initialized");
        return false;
    }

    FsFile file = sd.open(fileName.c_str(), O_WRITE | O_CREAT | O_APPEND);
    if (!file)
    {
        Serial.print("Failed to open file: ");
        Serial.println(fileName);
        return false;
    }

    file.println(data);
    file.close();

    Serial.print("Data written to ");
    Serial.print(fileName);
    Serial.print(": ");
    Serial.println(data);

    return true;
}

bool SDCardManager::createFile(String fileName)
{
    if (!initialized)
    {
        Serial.println("SD card not initialized");
        return false;
    }

    FsFile file = sd.open(fileName.c_str(), O_WRITE | O_CREAT);
    if (!file)
    {
        Serial.print("Failed to create file: ");
        Serial.println(fileName);
        return false;
    }

    file.close();
    Serial.print("File created: ");
    Serial.println(fileName);
    return true;
}

bool SDCardManager::deleteFile(String fileName)
{
    if (!initialized)
    {
        Serial.println("SD card not initialized");
        return false;
    }

    if (sd.remove(fileName.c_str()))
    {
        Serial.print("File deleted: ");
        Serial.println(fileName);
        return true;
    }
    else
    {
        Serial.print("Failed to delete file: ");
        Serial.println(fileName);
        return false;
    }
}

bool SDCardManager::fileExists(String fileName)
{
    if (!initialized)
    {
        return false;
    }

    return sd.exists(fileName.c_str());
}

long SDCardManager::getFileSize(String fileName)
{
    if (!initialized)
    {
        return -1;
    }

    FsFile file = sd.open(fileName.c_str(), O_READ);
    if (!file)
    {
        return -1;
    }

    long size = file.size();
    file.close();
    return size;
}

void SDCardManager::setLogFileName(String fileName)
{
    logFileName = fileName;
}

String SDCardManager::getLogFileName()
{
    return logFileName;
}

void SDCardManager::listFiles()
{
    if (!initialized)
    {
        Serial.println("SD card not initialized");
        return;
    }

    Serial.println("Files on SD card:");
    Serial.println("==================");

    FsFile root = sd.open("/");
    FsFile file;

    while (file.openNext(&root, O_READ))
    {
        if (!file.isHidden())
        {
            // Print file name
            char fileName[64];
            file.getName(fileName, sizeof(fileName));
            Serial.print(fileName);

            if (file.isDir())
            {
                Serial.println("/");
            }
            else
            {
                // Print file size
                Serial.print(" - ");
                long fileSize = file.size();
                if (fileSize < 1024)
                {
                    Serial.print(fileSize);
                    Serial.println(" bytes");
                }
                else if (fileSize < 1024 * 1024)
                {
                    Serial.print(fileSize / 1024.0, 1);
                    Serial.println(" KB");
                }
                else
                {
                    Serial.print(fileSize / (1024.0 * 1024.0), 2);
                    Serial.println(" MB");
                }
            }
        }
        file.close();
    }
    root.close();
    Serial.println("==================");
}

void SDCardManager::testSPIConnection()
{
    Serial.println("=== Shared SPI Connection Test ===");
    Serial.println("Testing SPI communication with shared pins...");

    // Ensure LoRa is deselected
    pinMode(PA4, OUTPUT);
    digitalWrite(PA4, HIGH);
    Serial.print("LoRa CS (PA4): ");
    Serial.println(digitalRead(PA4) ? "HIGH (Deselected)" : "LOW (Selected)");

    // Set SD CS pin as output and test
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);
    delay(10);
    digitalWrite(csPin, LOW);
    delay(10);
    digitalWrite(csPin, HIGH);

    Serial.print("SD CS (PB4/pin ");
    Serial.print(csPin);
    Serial.print("): ");
    Serial.println(digitalRead(csPin) ? "HIGH (Deselected)" : "LOW (Selected)");

    // Test shared SPI pins
    Serial.println("Shared SPI Pin Status:");
    Serial.print("  PA5 (SCK):  ");
    Serial.print(digitalRead(PA5) ? "HIGH" : "LOW");
    Serial.println(" - Clock line shared by LoRa & SD");
    Serial.print("  PA6 (MISO): ");
    Serial.print(digitalRead(PA6) ? "HIGH" : "LOW");
    Serial.println(" - Data from device (shared)");
    Serial.print("  PA7 (MOSI): ");
    Serial.print(digitalRead(PA7) ? "HIGH" : "LOW");
    Serial.println(" - Data to device (shared)");

    Serial.println("SPI Sharing Summary:");
    Serial.println("  LoRa uses: PA4(CS), PA5(SCK), PA6(MISO), PA7(MOSI)");
    Serial.println("  SD uses:   PB4(CS), PA5(SCK), PA6(MISO), PA7(MOSI)");
    Serial.println("  Only CS pins differ - this should work!");
    Serial.println("===================================");
}
