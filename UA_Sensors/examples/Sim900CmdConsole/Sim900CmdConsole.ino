/*
  Send AT Commands to Sim900 via Serial

  Sketch can be used to configure Sim900 shield

  Notes:
  - Make sure shield is manually powered on (ie, red light is on)
  - Notice that the baud rate for the shield connection is 4800. This
    is a workaround for SMS messages greater than 64 characters. Another
    option is to increase the buffer size defined in the Software Serial
    library.
  - The GSM shield will need to be set to listen at this baud rate. The
    AT command to use is "AT+IPR=4800"

  Created 14 6 2014
  Modified 22 6 2014
*/
#include <SoftwareSerial.h>

// GSM Shield uses software serial on pins 7, 8. Need to
// switch jumpers and do some soldering to enable software serial.
//
// NOTE: Software serial does not seem to work with this shield using
// 8 Mhz Arduinos
//
SoftwareSerial Sim900(7, 8);

const int MAX_BUFFER_SIZE = 100;

void setup()
{
  Serial.begin(19200);
  Sim900.begin(19200);

  Serial.println("Sim900 Command Console");
  Serial.println("----------------------");
}


void loop()
{
  char buffer[MAX_BUFFER_SIZE];
  int inputLength = 0;

  Serial.println("Enter AT command: ");

  while (!Serial.available())
  {
    delay(100);
  }

  inputLength = Serial.readBytesUntil('\n', buffer, MAX_BUFFER_SIZE - 1);
  buffer[inputLength] = '\0';

  Sim900.println(buffer);

  // Give some time for GSM shield to process command
  delay(500);

  // Listen for input from GSM shield.
  while (Sim900.available())
  {
    Serial.write(Sim900.read());
  }
}