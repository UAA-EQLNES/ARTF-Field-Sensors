/*
  Test Ultrasonic sensor readings

  Created 2 7 2014
  Modified 2 7 2014
*/

// Ultrasonic sensor settings
const byte ULTRASONIC_PIN = A6;

void setup()
{
  Serial.begin(9600);
}


void loop()
{
  Serial.println(analogRead(ULTRASONIC_PIN));
  delay(1000);
}