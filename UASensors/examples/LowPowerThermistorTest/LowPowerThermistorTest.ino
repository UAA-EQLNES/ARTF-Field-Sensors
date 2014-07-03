/*
  Test Thermistor sensor with Low Power library sleep

  This sketch specifically tests the thermistor used
  on our sensor platform.

  This sketch will blink n + 1 times for each wake up.

  If no blinks are seen or only a single blink, then the board
  is reseting or not reading correctly.

  You should detach the SD Card and RTC breakout boards as well
  as the GSM shield. The sketch needs to be run from the battery.

  The thermistor readings are taken from analog pin **5**.

  Created 1 7 2014
  Modified 2 7 2014
*/

#include <LowPower.h>

// LED blink settings
const byte LED = 13;
const int BLINK_DELAY = 200;

// Thermistor settings
const byte THEMISTOR_PIN = A5;
const byte THERM_PWR_PIN = 7;

int wakeupCount = 0;

void setup()
{
  pinMode(THERM_PWR_PIN, OUTPUT);
}


void loop()
{
  // Sleep for about 8 seconds
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  wakeupCount += 1;

  // Turn on thermistor power
  digitalWrite(THERM_PWR_PIN, HIGH);
  delay(500);

  // Take a reading from the thermistor
  int reading = analogRead(THEMISTOR_PIN);

  // Blink if we got a reading that does not equal 0
  if (reading != 0)
  {
    // Blink n times for the number of wakes up counted
    for (int i = 0; i < wakeupCount; ++i)
    {
      blink(LED, BLINK_DELAY);
    }
  }

  // Turn off thermistor power
  digitalWrite(THERM_PWR_PIN, LOW);
  delay(500);
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