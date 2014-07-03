#ifndef _UASENSORS_SDCARD_H_
#define _UASENSORS_SDCARD_H_

#include <Arduino.h>
#include <String.h>
#include <SdFat.h>

class UASensors_SDCard {
public:
    UASensors_SDCard(int csPin) : _csPin(csPin) {};
    void begin();
    bool writeFile(char *filename, String data);
    String readFile(char *filename);
private:
    SdFat _sd;
    SdFile _file;
    int _csPin;
};

#endif