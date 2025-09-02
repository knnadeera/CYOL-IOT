#include "LEDControl.h"

void runLEDPattern(int ledPin)
{
    const int pattern[] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 4};
    int patternLength = sizeof(pattern) / sizeof(pattern[0]);
    for (int i = 0; i < patternLength; i++)
    {
        digitalWrite(ledPin, pattern[i]);
        delay(pattern[i] == 4 ? 500 : 50);
    }
    digitalWrite(ledPin, LOW);
}

void blinkLED(int ledPin, int delayTime)
{
    digitalWrite(ledPin, HIGH);
    delay(delayTime);
    digitalWrite(ledPin, LOW);
    delay(delayTime);
}
