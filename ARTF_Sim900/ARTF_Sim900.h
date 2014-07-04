#ifndef _ARTF_SIM900_H_
#define _ARTF_SIM900_H_

#include <Arduino.h>
#include <String.h>

class ARTF_Sim900 {
public:
    static const int POWER_PIN = 9;
    static const int MAX_POWER_RETRIES = 4;
    static const int DEFAULT_POWER_TIMEOUT = 250;
    static const int DEFAULT_SMS_TIMEOUT = 100;
    static const int POWER_OFFLINE = 0;
    static const int POWER_READY = 1;
    static const int POWER_UNKNOWN = 2;

    void begin(Stream *serial);
    void togglePower();
    bool isReady();
    bool isOffline();
    bool isUnknownState();
    bool ensureReady();
    bool ensureOffline();
    void waitPowerToggleCompleted();
    void waitPowerToggleCompleted(int timeout);
    void clearBuffer();
    void sendTextMsg(String msg, String phoneNumber);
    bool isTextMsgDelivered();
    bool isTextMsgDelivered(int timeout);
    void writeAtCommand(String command);
    int read();
    int available();
private:
    bool _ensurePowerStatus(int status, int maxRetries);
    Stream *_serial;
    int _powerStatus;
};

#endif