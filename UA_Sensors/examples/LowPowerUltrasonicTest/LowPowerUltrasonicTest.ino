/*
  Test Ultrasonic sensor with Low Power library sleep

  This sketch specifically tests the ultrasonic sensor used
  on our sensor platform.

  This sketch will blink n + 1 times for each wake up.

  If no blinks are seen or only a single blink, then the board
  is reseting or not reading correctly.

  You should detach the SD Card and RTC breakout boards as well
  as the GSM shield. The sketch needs to be run from the battery.

  There is a MOSFET controlled by pin **5** that needs to be
  turned high for the ultrasonic sensor to take valid readings.

  The Ultrasonic readings are taken from analog pin **6**.

  Notes:
  - Had a case where the RocketScream would reset when MOSFET pin
    was powered high.

  Created 1 7 2014
  Modified 2 7 2014
*/

#include <LowPower.h>

// LED blink settings
const byte LED = 13;
const int BLINK_DELAY = 200;

// Ultrasonic sensor settings
const byte ULTRASONIC_PIN = A6;
const byte MOSFET_US_PIN = 5;

int wakeupCount = 0;

void setup()
{

}


void loop()
{
  // Sleep for about 8 seconds
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  wakeupCount += 1;

  // Turn on MOSFET
  digitalWrite(MOSFET_US_PIN, HIGH);
  delay(3000);

  // Take a reading from the Ultrasonic sensor
  int reading = analogRead(ULTRASONIC_PIN);

  // Blink twice if we got reading that was greater than 0
  if (reading > 0)
  {
    // Blink n times for the number of wakes up counted
    for (int i = 0; i < wakeupCount; ++i)
    {
      blink(LED, BLINK_DELAY);
    }
  }

  // Turn off MOSFET
  digitalWrite(MOSFET_US_PIN, LOW);
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