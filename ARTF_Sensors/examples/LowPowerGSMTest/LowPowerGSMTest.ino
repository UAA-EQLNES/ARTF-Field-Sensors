/*
  Test GSM Shield sensor with Low Power library sleep

  This sketch specifically tests the LinkSprite Sim900 GSM shield
  used on our sensor platform.

  This sketch will send a text message saying "Hello World " + n,
  where n is the number of wake ups counted.

  You should detach the SD Card and RTC breakout boards. The sketch
  needs to be run from the battery.

  Created 1 7 2014
  Modified 2 7 2014
*/

#include <LowPower.h>

#include <ARTF_Sim900.h>


// LED blink settings
const byte LED = 13;
const int BLINK_DELAY = 200;

// GSM shield settings
const byte MOSFET_GSM_PIN = 6;
const int SEND_DATA_EVERY_X_WAKEUPS = 5;

// Settings
#define PHONE_NUMBER "+12223334444"


int wakeupCount = 0;
ARTF_Sim900 sim900;


void setup()
{
  sim900.begin(&Serial);
}


void loop()
{
  // Sleep for about 8 seconds
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  wakeupCount += 1;

  if (wakeupCount % SEND_DATA_EVERY_X_WAKEUPS == 0)
  {
    // Turn on GSM MOSFET
    digitalWrite(MOSFET_GSM_PIN, HIGH);
    delay(1000);

    // GSM baud rate. Start communication link with GSM shield.
    Serial.begin(19200);
    delay(100);

    if (sim900.ensureReady() == true)
    {
      String textMessage = "Hello World " + String(wakeupCount);
      sim900.sendTextMsg(textMessage, PHONE_NUMBER);
      if (sim900.isTextMsgDelivered() == false)
      {
        // If failed to send text message, blink three times
        blink(LED, BLINK_DELAY);
        blink(LED, BLINK_DELAY);
        blink(LED, BLINK_DELAY);
      }
    }
    else
    {
      // If GSM did not power on, blink twice
      blink(LED, BLINK_DELAY);
      blink(LED, BLINK_DELAY);
    }

    // Turn off GSM power pin.
    if (sim900.ensureOffline() == false)
    {
      // If we can't turn off the GSM, then blink once
      blink(LED, BLINK_DELAY);
    }

    // Turn off GSM MOSFET
    digitalWrite(MOSFET_GSM_PIN, LOW);
    delay(1000);
  }
}

// Simple blink function
void blink(byte pin, int delay_ms)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(delay_ms);
  digitalWrite(pin, LOW);
  delay(delay_ms);
}