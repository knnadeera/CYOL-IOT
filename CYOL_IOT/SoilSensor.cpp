#include "SoilSensor.h"

int readSoilMoisture(int analogPin, int digitalPin)
{
    int analogValue = analogRead(analogPin);
    int digitalValue = digitalRead(digitalPin);
    if (analogValue < 0 || analogValue > 1023)
    {
        return -1;
    }
    return analogValue;
}
