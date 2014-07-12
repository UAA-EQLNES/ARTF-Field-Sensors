/*
  Moisture Sensor with GSM, SDCard, and RTC

  Sketch used by ARTF Sensors platform.

  Notes:
  - This is an early implementation. Parts of code need to be cleaned up
    to remove duplication. But should work for now.
  - Use 930 MOISTURE_MAX_READING for 3.3v Arduino. About 610 for 5v Arduino
  - Had to remove RTC time for sample GSM message. Red light on RocketScream stays on...

  Created 8 7 2014
  Modified 11 7 2014
*/

#include <LowPower.h>
#include <math.h>

#include <ARTF_Sim900.h>
#include <ARTF_SDCard.h>
#include <ARTF_RTC.h>


// ARTF SDCard Dependency
#include <SdFat.h>
#include <String.h>

// ARTF RTC Dependency
#include <SPI.h>
#include <Time.h>


// Analog Pins
#define MOISTURE_PIN 6


// Digital Pins
#define MOSFET_MS_PIN    5
#define MOSFET_GSM_PIN   6
#define RTC_CS_PIN       8
#define GSM_PWRKEY       9
#define SD_CS_PIN        10


// Settings
// -------------------------------
#define SENSOR_TYPE                "s"
#define MOISTURE_MAX_READING       930
#define SEND_DATA_AFTER_X_READINGS 3
#define SLEEP_CYCLES               3600
#define NUM_MOISTURE_READINGS      10
#define MOISTURE_READING_DELAY     100
#define DATA_DELIM                 ';'
#define BACKUP_FILENAME            "backup.txt"
#define UNSENT_FILENAME            "unsent.txt"
#define ERROR_FILENAME             "error.txt"
#define PHONE_NUMBER               "+12223334444"
#define ERROR_GSM                  "GSM Failed"
#define ERROR_SMS                  "SMS Failed"

#define TEST_PHONE_NUMBER          "+12223334444"

// Custom Datatypes
typedef struct {
  int moisture;
  time_t timestamp;
} SensorReading;


// Global Variables
int numCachedReadings = 0;
int totalReadings = 0;
SensorReading sensorReadings[SEND_DATA_AFTER_X_READINGS];
ARTF_Sim900 sim900;
ARTF_RTC rtc(RTC_CS_PIN);
ARTF_SDCard sd(SD_CS_PIN);


void setup()
{
  pinMode(SD_CS_PIN, OUTPUT);
  sim900.begin(&Serial);
  delay(300);


  // 0. Send a test text message with with moisture reading and time
  // ---------------------------------------------------------------
  int roundedMoisture = takeMoistureReadings();
  String textMessage = "Test: " + String(SENSOR_TYPE) + " " + String(roundedMoisture);

  digitalWrite(MOSFET_GSM_PIN, HIGH);
  delay(1500);

  Serial.begin(19200);
  delay(100);

  if (sim900.ensureReady() == true)
  {
    sim900.sendTextMsg(textMessage, TEST_PHONE_NUMBER);
    sim900.isTextMsgDelivered();
  }
  sim900.ensureOffline();

  digitalWrite(MOSFET_GSM_PIN, LOW);
  delay(2000);

}


void loop()
{

  // 1. Wake up.
  // -----------

  // Enter idle state for 8 s with the rest of peripherals turned off.
  // 98 sleep cycles will take a reading approximately every 13 minutes.
  for (int i = 0; i < SLEEP_CYCLES; ++i)
  {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  }

  // 3.,4.,5.,6. Takes X moisture readings and averages them
  // -------------------------------------------------------
  int roundedMoisture = takeMoistureReadings();


  // 7. Get time from RTC Shield.
  // -----------------------------
  rtc.begin();
  time_t unixTime = rtc.readDateTime();


  // 8. Combine time, moisture into a single string.
  // -----------------------------------------------
  totalReadings += 1;
  String dataString = String(totalReadings) + " " + String(unixTime) + " " + String(roundedMoisture);

  // Cache distance and time in global array variable
  sensorReadings[numCachedReadings].moisture = roundedMoisture;
  sensorReadings[numCachedReadings].timestamp = unixTime;
  numCachedReadings += 1;


  // 9. Turn on SD Card.
  // 10. Save data string to text file on SD card
  // --------------------------------------------

  // Try to turn on SD card. Should only need to be called once.
  sd.begin();
  sd.writeFile(BACKUP_FILENAME, dataString);
  delay(1000);


  // 11. Are there X unsent data strings?
  // 12. Yes. Send X unsent data strings in one SMS. Go to 13.
  // -------------------------------------
  if (numCachedReadings == SEND_DATA_AFTER_X_READINGS)
  {

    // 13. Prepare text message
    // ---------------------
    String textMessage = String(SENSOR_TYPE) + " " +
      String(sensorReadings[0].timestamp) + " " +
      String(sensorReadings[0].moisture);

    time_t startTime = sensorReadings[0].timestamp;
    int minutesElapsed = 0;

    // Format data to send as text message
    for (int i = 1; i < numCachedReadings; ++i)
    {
      minutesElapsed = (sensorReadings[i].timestamp - startTime) / 60;
      textMessage += String(DATA_DELIM) + String(minutesElapsed) + " " + String(sensorReadings[i].moisture);
    }

    // Turn on MOSFET GSM
    digitalWrite(MOSFET_GSM_PIN, HIGH);
    delay(1500);


    // GSM baud rate. Start communication link with GSM shield.
    Serial.begin(19200);
    delay(100);


    // 14. Send text message if GSM Ready
    if (sim900.ensureReady() == true)
    {
      sim900.sendTextMsg(textMessage, PHONE_NUMBER);
      if (sim900.isTextMsgDelivered() == false)
      {
        sd.begin();
        sd.writeFile(ERROR_FILENAME, String(unixTime) + ": " + ERROR_SMS);
        sd.writeFile(UNSENT_FILENAME, textMessage);
      }
    }
    else
    {
      sd.begin();
      sd.writeFile(ERROR_FILENAME, String(unixTime) + ": " + ERROR_GSM);
      sd.writeFile(UNSENT_FILENAME, textMessage);
    }

    // Reset number of cached readings
    numCachedReadings = 0;


    // 15. Turn off GSM.
    // -----------------

    if (sim900.ensureOffline() == false)
    {
      // TODO(richard-to): What would be best way to handle this issue?
      //                   Battery will die quickly if power cannot be turned off.
      //                   Send SOS SMS?
    }

    // Turn off GSM MOSFET.
    digitalWrite(MOSFET_GSM_PIN, LOW);
    delay(2000);
  }
}


int takeMoistureReadings()
{
  // 2. Turn on moisture sensor (MOSFET).
  // --------------------------------------
  digitalWrite(MOSFET_MS_PIN, HIGH);
  // Calibration time
  delay(2000);


  // 3. Take X moisture readings. (One every 200ms)
  // ----------------------------------------------
  double moistureReadings[NUM_MOISTURE_READINGS];
  for (int i = 0; i < NUM_MOISTURE_READINGS; ++i)
  {
    moistureReadings[i] = (double)analogRead(MOISTURE_PIN) / (double)MOISTURE_MAX_READING;
    delay(MOISTURE_READING_DELAY);
  }


  // 4. Turn off moisture sensor (MOSFET).
  // ---------------------------------------
  digitalWrite(MOSFET_MS_PIN, LOW);
  delay(500);


  // 5. Average X moisture measurements.
  // ------------------------------------
  double sumMoisture = 0.0;
  for (int i = 0; i < NUM_MOISTURE_READINGS; ++i)
  {
    sumMoisture += moistureReadings[i];
  }
  double avgMoisture = sumMoisture / NUM_MOISTURE_READINGS;


  // 6. Round moisture reading
  // -------------------------
  int roundedMoisture = round(avgMoisture * 100);

  return roundedMoisture;
}
