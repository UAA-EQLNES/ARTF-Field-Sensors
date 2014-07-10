/*
  Test Ultrasonic sensor readings

  This sketch takes 5 readings and averages them to
  help verify similar calcuations used in BridgeSensorGSM
  sketch.

  Created 2 7 2014
  Modified 10 7 2014
*/


// Ultrasonic sensor settings
const byte ULTRASONIC_PIN = A6;
const int DISTANCE_INCREMENT = 5;
const int NUM_READINGS = 5;


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

  // Take X readings
  int distanceReadings[NUM_READINGS];
  for (int i = 0; i < NUM_READINGS; ++i)
  {
    int reading = analogRead(ULTRASONIC_PIN);
    distanceReadings[i] = reading * DISTANCE_INCREMENT;

    Serial.print(i);
    Serial.print(". Analog:");
    Serial.print(reading);
    Serial.print("; Calculated:");
    Serial.println(reading * DISTANCE_INCREMENT);

    delay(300);
  }

  // Average the readings
  double sumDistance = 0.0;
  for (int i = 0; i < NUM_READINGS; ++i)
  {
    sumDistance += distanceReadings[i];
  }
  double avgDistance = sumDistance / NUM_READINGS;

  // Rounded measurements
  int roundedDistance = round(avgDistance);


  Serial.print("Sum:");
  Serial.println(sumDistance);

  Serial.print("Avg:");
  Serial.println(avgDistance);

  Serial.print("Rounded:");
  Serial.println(roundedDistance);

  delay(500);
  count += 1;
}