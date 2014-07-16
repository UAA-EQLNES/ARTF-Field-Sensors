#include "ARTF_Sim900.h"
#include <Arduino.h>
#include <String.h>

void ARTF_Sim900::begin(Stream *serial)
{
    _serial = serial;
    _powerStatus = POWER_OFFLINE;
}

void ARTF_Sim900::togglePower()
{
  digitalWrite(POWER_PIN, HIGH);
  delay(1000);
  digitalWrite(POWER_PIN, LOW);
  delay(1000);
}

bool ARTF_Sim900::ensureReady()
{
    return _ensurePowerStatus(POWER_READY, MAX_POWER_RETRIES);
}

bool ARTF_Sim900::ensureOffline()
{
    return _ensurePowerStatus(POWER_OFFLINE, MAX_POWER_RETRIES);
}

bool ARTF_Sim900::_ensurePowerStatus(int status, int maxRetries)
{
    bool success = false;
    int retries = 0;

    while (retries < maxRetries && success == false)
    {
        togglePower();
        waitPowerToggleCompleted();

        if (_powerStatus == status)
        {
            success = true;
        }
        else
        {
            retries += 1;
        }
        delay(300);
    }
    return success;
}

bool ARTF_Sim900::isReady()
{
    return _powerStatus == POWER_READY;
}

bool ARTF_Sim900::isOffline()
{
    return  _powerStatus == POWER_OFFLINE;
}

bool ARTF_Sim900::isUnknownState()
{
    return  _powerStatus == POWER_UNKNOWN;
}

void ARTF_Sim900::waitPowerToggleCompleted()
{
    waitPowerToggleCompleted(DEFAULT_POWER_TIMEOUT);
}

void ARTF_Sim900::waitPowerToggleCompleted(int timeout)
{
    char gsmReady[] = "Call Ready";
    char gsmReadyLength = 10;

    char gsmOffline[] = "NORMAL POWER DOWN";
    char gsmOfflineLength = 17;

    int matchedOffline = 0;
    int matchedReady = 0;

    char c;

    while (timeout > 0)
    {
        while (_serial->available())
        {
            c = _serial->read();
            if (c == gsmOffline[matchedOffline])
            {
                matchedOffline += 1;
                matchedReady = 0;
                if (matchedOffline == gsmOfflineLength)
                {
                    _powerStatus = POWER_OFFLINE;
                    return;
                }
            }
            else if (c == gsmReady[matchedReady])
            {
                matchedReady += 1;
                matchedOffline = 0;
                if (matchedReady == gsmReadyLength)
                {
                    _powerStatus = POWER_READY;
                    return;
                }
            } else {
                matchedOffline = 0;
                matchedReady = 0;
            }
        }
        timeout -= 1;
        delay(100);
    }
    _powerStatus = POWER_UNKNOWN;
}

bool ARTF_Sim900::isTextMsgDelivered()
{
    return isTextMsgDelivered(DEFAULT_SMS_TIMEOUT);
}

bool ARTF_Sim900::isTextMsgDelivered(int timeout)
{
    char smsOK[] = "OK";
    int smsOKLength = 2;
    int numOKNeeded = 2;

    char smsError[] = "ERROR";
    int smsErrorLength = 5;

    char c;

    int matchedError = 0;
    int matchedOK = 0;
    int foundOK = 0;

    while (timeout > 0)
    {
        while (_serial->available())
        {
            c = _serial->read();
            if (c == smsError[matchedError])
            {
                matchedError += 1;
                matchedOK = 0;
                if (matchedError == smsErrorLength)
                {
                    return false;
                }
            }
            else if (c == smsOK[matchedOK])
            {
                matchedOK += 1;
                matchedError = 0;
                if (matchedOK == smsOKLength)
                {
                    foundOK += 1;
                }

                if (foundOK == smsOKLength)
                {
                    return true;
                }
            } else {
                matchedOK = 0;
                matchedError = 0;
            }
        }
        timeout -= 1;
        delay(100);
    }
    return false;
}

void ARTF_Sim900::clearBuffer()
{
    char c;
    while (_serial->available() > 0)
    {
        c = _serial->read();
    }
}

void ARTF_Sim900::sendTextMsg(char *msg, char *phoneNumber)
{
    _serial->print(F("AT+CMGF=1\r"));
    delay(500);

    _serial->print(F("AT + CMGS = \""));
    _serial->print(phoneNumber);
    _serial->println(F("\""));
    delay(500);

    _serial->println(msg);
    delay(500);

    _serial->println((char)26);
    delay(500);
}

void ARTF_Sim900::sendTextMsg(String msg, char *phoneNumber)
{
    _serial->print(F("AT+CMGF=1\r"));
    delay(500);

    _serial->print(F("AT + CMGS = \""));
    _serial->print(phoneNumber);
    _serial->println(F("\""));
    delay(500);

    _serial->println(msg);
    delay(500);

    _serial->println((char)26);
    delay(500);
}

int ARTF_Sim900::read()
{
    return _serial->read();
}

int ARTF_Sim900::available()
{
    return _serial->available();
}

void ARTF_Sim900::writeAtCommand(String command)
{
    _serial->println(command);
    delay(100);
}