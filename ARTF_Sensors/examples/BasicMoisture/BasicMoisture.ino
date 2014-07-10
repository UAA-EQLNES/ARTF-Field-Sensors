/*
  Basic test for moisture sensor readings

  This sketch takes 5 readings and averages them to
  help verify similar calcuations used in MoistureSensorGSM
  sketch.

  Created 10 7 2014
  Modified 10 7 2014
*/


// Moisture sensor settings
const byte MOISTURE_PIN = A6;
const int NUM_MOISTURE_READINGS = 5;
const int MOISTURE_MAX_READING = 600;


void setup()
{
  Serial.begin(9600);
}

int count = 1;
void loop()
{
  Serial.print("Trial ");
  Serial.println(count);
  Serial.println("-------------------");

  // Take X readings and calculate percentage
  double moistureReadings[NUM_MOISTURE_READINGS];
  for (int i = 0; i < NUM_MOISTURE_READINGS; ++i)
  {
    int reading = analogRead(MOISTURE_PIN);
    moistureReadings[i] = (double)reading / (double)MOISTURE_MAX_READING;

    Serial.print(i);
    Serial.print(". Analog:");
    Serial.print(reading);
    Serial.print("; Percent:");
    Serial.print(((double)reading / (double)MOISTURE_MAX_READING));

    delay(300);
  }

  // Average the readings
  double sumMoisture = 0.0;
  for (int i = 0; i < NUM_MOISTURE_READINGS; ++i)
  {
    sumMoisture += moistureReadings[i];
  }
  double avgMoisture = sumMoisture / NUM_MOISTURE_READINGS;

  // Rounded measurements
  int roundedMoisture = round(avgMoisture * 100);


  Serial.print("Sum:");
  Serial.println(sumMoisture);

  Serial.print("Avg:");
  Serial.println(avgMoisture);

  Serial.print("Rounded:");
  Serial.println(roundedMoisture);

  delay(500);
  count += 1;
}