#include "SIM7600Module.h"

bool sendCommandCheck(HardwareSerial &sim7600, String command, String expectedResponse, int timeout)
{
    sim7600.println(command);
    unsigned long startTime = millis();
    while (millis() - startTime < timeout)
    {
        if (sim7600.available())
        {
            String response = sim7600.readString();
            if (response.indexOf(expectedResponse) != -1)
            {
                return true;
            }
        }
    }
    return false;
}

bool checkModuleReady(HardwareSerial &sim7600)
{
    if (!sendCommandCheck(sim7600, "AT", "OK"))
        return false;
    if (!sendCommandCheck(sim7600, "AT+CPIN?", "READY"))
        return false;
    if (!sendCommandCheck(sim7600, "AT+CGREG?", "0,1") && !sendCommandCheck(sim7600, "AT+CGREG?", "0,5"))
        return false;
    return true;
}

void sendPostRequest(HardwareSerial &sim7600, const String &backendURL, const String &jsonData)
{
    sim7600.println("AT+HTTPINIT");
    delay(1000);
    sim7600.println("AT+HTTPPARA=\"CID\",1");
    delay(1000);
    sim7600.println("AT+HTTPPARA=\"URL\",\"" + backendURL + "\"");
    delay(1000);
    sim7600.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
    delay(1000);
    sim7600.println("AT+HTTPDATA=" + String(jsonData.length()) + ",10000");
    delay(500);
    sim7600.print(jsonData);
    delay(500);
    sim7600.println("AT+HTTPACTION=1");
    delay(5000);
    sim7600.println("AT+HTTPREAD");
    delay(2000);
}
