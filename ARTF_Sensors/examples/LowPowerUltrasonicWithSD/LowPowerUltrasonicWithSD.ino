/*
  Test Ultrasonic sensor readings on sensor platform

  This sketch is designed to test the accuracy of the Ultrasonic
  sensor with the battery pack and circuit of the sensor platform.

  This sketch takes 5 readings and averages them to
  help verify similar calculations used in BridgeSensorGSM
  sketch. The results are written to the SD card

  Created 10 7 2014
  Modified 10 7 2014
*/
#include <LowPower.h>

#include <ARTF_SDCard.h>

// ARTF SDCard Dependency
#include <SdFat.h>
#include <String.h>


// Ultrasonic Settings
const byte ULTRASONIC_PIN = A6;
const byte MOSFET_US_PIN = 5;
const int DISTANCE_INCREMENT = 5;
const int NUM_READINGS = 5;

// SD Card Settings
const byte SD_CS_PIN = 10;
#define OUTPUT_FILENAME "ultra.txt"


ARTF_SDCard sd(SD_CS_PIN);

void setup()
{
  Serial.begin(9600);
  pinMode(SD_CS_PIN, OUTPUT);
}

int count = 1;
void loop()
{

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  String output = "";

  output += "Trial " + String(count) + "\n";
  output += "-------------------\n";

  digitalWrite(MOSFET_US_PIN, HIGH);
  delay(3000);


  // Take X readings
  int distanceReadings[NUM_READINGS];
  for (int i = 0; i < NUM_READINGS; ++i)
  {
    int reading = analogRead(ULTRASONIC_PIN);
    distanceReadings[i] = reading * DISTANCE_INCREMENT;

    output += String(i) + ". Analog:" + String(reading) + "; Calculated:" + String(distanceReadings[i]) + "\n";

    delay(300);
  }

  digitalWrite(MOSFET_US_PIN, LOW);
  delay(500);

  // Average the readings
  double sumDistance = 0.0;
  for (int i = 0; i < NUM_READINGS; ++i)
  {
    sumDistance += distanceReadings[i];
  }
  double avgDistance = sumDistance / NUM_READINGS;

  // Rounded measurements
  int roundedDistance = round(avgDistance);

  output += "Rounded:" + String(roundedDistance) + "\n\n";




  sd.begin();
  sd.writeFile(OUTPUT_FILENAME, output);

  delay(500);
  count += 1;
}