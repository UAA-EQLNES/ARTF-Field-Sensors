/*
  Test Moisture sensor readings on sensor platform

  This sketch is designed to test the accuracy of the Moisture
  sensor with the battery pack and circuit of the sensor platform.

  This sketch takes 5 readings and averages them to
  help verify similar calculations used in MoistureSensorGSM
  sketch. The results are written to the SD card

  Created 10 7 2014
  Modified 10 7 2014
*/
#include <LowPower.h>

#include <ARTF_SDCard.h>

// ARTF SDCard Dependency
#include <SdFat.h>
#include <String.h>


// Moisture sensor settings
const byte MOISTURE_PIN = A6;
const byte MOSFET_MS_PIN = 5;
const int NUM_READINGS = 5;
const int MOISTURE_MAX_READING = 600;


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


  digitalWrite(MOSFET_MS_PIN, HIGH);
  delay(3000);

  // Take X readings
  int moistureReadings[NUM_READINGS];
  for (int i = 0; i < NUM_READINGS; ++i)
  {
    int reading = analogRead(MOISTURE_PIN);
    moistureReadings[i] = (double)reading / (double)MOISTURE_MAX_READING;

    output += String(i) + ". Analog:" + String(reading) + "; Calculated:" + String(moistureReadings[i]) + "\n";

    delay(300);
  }

  digitalWrite(MOSFET_MS_PIN, LOW);
  delay(500);


  // Average the readings
  double sumMoisture = 0.0;
  for (int i = 0; i < NUM_READINGS; ++i)
  {
    sumMoisture += moistureReadings[i];
  }
  double avgMoisture = sumMoisture / NUM_READINGS;

  // Rounded measurements
  int roundedMoisture = round(avgMoisture * 100);

  output += "Rounded:" + String(roundedMoisture) + "\n\n";

  sd.begin();
  sd.writeFile(OUTPUT_FILENAME, output);

  delay(500);
  count += 1;
}