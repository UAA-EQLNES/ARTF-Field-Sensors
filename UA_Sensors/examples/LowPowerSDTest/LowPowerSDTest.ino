/*
  Test SD Card Shield sensor with Low Power library sleep

  This sketch specifically tests the DeadOn RTC - DS3234 Breakout board
  used on our sensor platform.

  This sketch will write a value of n + 1 to the file test.txt each time
  the RocketScream wakes up.

  You should detach the RTC breakout board and GSM Shield.

  Created 1 7 2014
  Modified 2 7 2014
*/

#include <LowPower.h>

#include <UASensors_SDCard.h>

// UASensors SDCard Dependency
#include <SdFat.h>


// SD card settings
const byte SD_CS_PIN = 10;

// Settings
#define TEST_FILENAME "test.txt"

int wakeupCount = 0;
UASensors_SDCard sd(SD_CS_PIN);


void setup()
{
  pinMode(SD_CS_PIN, OUTPUT);
}


void loop()
{
  // Sleep for about 8 seconds
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  wakeupCount += 1;

  sd.begin();
  sd.writeFile(TEST_FILENAME, String(wakeupCount));
  delay(1000);
}