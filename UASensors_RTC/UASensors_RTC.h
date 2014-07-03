#ifndef _UASENSORS_RTC_H_
#define _UASENSORS_RTC_H_

#include <Arduino.h>
#include <String.h>
#include <Time.h>

class UASensors_RTC {
public:
    UASensors_RTC(int csPin) : _csPin(csPin) {};
    void begin();
    void setDateTime(int day, int month, int year, int hours, int minutes, int seconds);
    String readDateTimeAsText();
    time_t readDateTime();
private:
    int _csPin;
};

#endif