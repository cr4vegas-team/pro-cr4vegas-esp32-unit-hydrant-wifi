#define SerialMon Serial

#include <Arduino.h>

const bool DEBUG = true;

void printDebug(String msg)
{
    if (DEBUG)
    {
        SerialMon.println(msg);
    }
}