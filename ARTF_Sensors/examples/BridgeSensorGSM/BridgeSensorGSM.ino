/*
  Bridge Sensor with GSM, SDCard, and RTC

  Sketch used by ARTF Sensors platform.

  Created 14 6 2014
  Modified 16 7 2014
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


// Constants
#define TEMP_BUFFER_SIZE 25
#define SMS_BUFFER_SIZE  160


// Analog Pins
#define THERMISTOR_PIN 5
#define ULTRASONIC_PIN 6


// Digital Pins
#define MOSFET_US_PIN    5
#define MOSFET_GSM_PIN   6
#define THERM_PIN        7
#define RTC_CS_PIN       8
#define GSM_PWRKEY       9
#define SD_CS_PIN        10


// Settings
// -------------------------------
#define SENSOR_TYPE                'd'
#define SENSOR_TO_RIVER_BED        1100
#define SEND_DATA_AFTER_X_READINGS 12
#define SLEEP_CYCLES               900
#define NUM_THERM_READINGS         5
#define THERM_READING_DELAY        20
#define NUM_DISTANCE_READINGS      3
#define DISTANCE_READING_DELAY     200
#define DISTANCE_INCREMENT         5
#define SMS_TIMEOUT                250
#define DATA_DELIM                 ';'
#define BACKUP_FILENAME            "backup.txt"
#define UNSENT_FILENAME            "unsent.txt"
#define ERROR_FILENAME             "error.txt"
#define PHONE_NUMBER               "+12223334444"
#define TEST_PHONE_NUMBER          "+12223334444"


// Custom Datatypes
typedef struct {
  int distance;
  int temperature;
  time_t timestamp;
} SensorReading;


// Global Variable
int sleepCount = 0;
int numCachedReadings = 0;
int totalReadings = 0;
SensorReading sensorReadings[SEND_DATA_AFTER_X_READINGS];
ARTF_Sim900 sim900;
ARTF_RTC rtc(RTC_CS_PIN);
ARTF_SDCard sd(SD_CS_PIN);

void setup()
{
  pinMode(SD_CS_PIN, OUTPUT);
  pinMode(THERM_PIN, OUTPUT);
  sim900.begin(&Serial);


  // 0. Send a test text message with temperature and distance readings
  // ------------------------------------------------------------------
  double temperature = takeThermReading();
  double distance = takeDistanceReading(temperature);
  int roundedTemperature = round(temperature);
  int roundedDistance = round(distance);

  char dataBuffer[TEMP_BUFFER_SIZE];
  snprintf(dataBuffer, TEMP_BUFFER_SIZE, "Test: %c %d %d",
    SENSOR_TYPE, roundedTemperature, roundedDistance);

  digitalWrite(MOSFET_GSM_PIN, HIGH);
  delay(1500);

  Serial.begin(19200);
  delay(100);

  if (sim900.ensureReady() == true)
  {
    sim900.sendTextMsg(dataBuffer, TEST_PHONE_NUMBER);
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
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  if (sleepCount < SLEEP_CYCLES)
  {
    sleepCount += 1;
    return;
  }
  sleepCount = 0;


  // Steps 2-6
  double temperature = takeThermReading();

  // Steps 7-11
  double distance = takeDistanceReading(temperature);

  int roundedTemperature = round(temperature);
  int roundedDistance = round(distance);


  // 12. Get time from RTC Shield.
  // -----------------------------
  rtc.begin();
  time_t unixTime = rtc.readDateTime();


  char dataBuffer[TEMP_BUFFER_SIZE];
  char smsBuffer[SMS_BUFFER_SIZE];


  // 13. Combine time, distance, and temperature into a single string.
  // -----------------------------------------------------------------
  totalReadings += 1;

  // Cache distance and time in global array variable
  sensorReadings[numCachedReadings].distance = roundedDistance;
  sensorReadings[numCachedReadings].temperature = roundedTemperature;
  sensorReadings[numCachedReadings].timestamp = unixTime;
  numCachedReadings += 1;


  // 14. Are there 4 unsent data strings?
  // 15. Yes. Send 4 unsent data strings in one SMS. Go to 18.
  // -------------------------------------
  if (numCachedReadings == SEND_DATA_AFTER_X_READINGS)
  {

    // 16. Prepare text message
    // ---------------------
    snprintf(smsBuffer, SMS_BUFFER_SIZE, "%c %d %d %d",
      SENSOR_TYPE, sensorReadings[0].timestamp, sensorReadings[0].distance, sensorReadings[0].temperature);

    time_t startTime = sensorReadings[0].timestamp;
    int minutesElapsed = 0;

    // Format data to send as text message
    for (int i = 1; i < numCachedReadings; ++i)
    {
      minutesElapsed = (sensorReadings[i].timestamp - startTime) / 60;
      snprintf(dataBuffer, TEMP_BUFFER_SIZE, "%c %d %d %d", DATA_DELIM, minutesElapsed, sensorReadings[0].distance, sensorReadings[0].temperature);
      strncat (smsBuffer, dataBuffer, strlen(dataBuffer));
    }

    // Turn on MOSFET GSM
    digitalWrite(MOSFET_GSM_PIN, HIGH);
    delay(1500);


    // GSM baud rate. Start communication link with GSM shield.
    Serial.begin(19200);
    delay(100);


    // 17. Send text message if GSM Ready
    if (sim900.ensureReady() == true)
    {
      sim900.sendTextMsg(smsBuffer, PHONE_NUMBER);
      if (sim900.isTextMsgDelivered(SMS_TIMEOUT) == false)
      {
        sd.begin();
        sd.writeFile(UNSENT_FILENAME, smsBuffer);
        snprintf(dataBuffer, TEMP_BUFFER_SIZE, "%d: SMS FAILED", unixTime);
        sd.writeFile(ERROR_FILENAME, dataBuffer);
      }
    }
    else
    {
      sd.begin();
      sd.writeFile(UNSENT_FILENAME, smsBuffer);
      snprintf(dataBuffer, TEMP_BUFFER_SIZE, "%d: GSM FAILED", unixTime);
      sd.writeFile(ERROR_FILENAME, dataBuffer);
    }

    // Reset number of cached readings
    numCachedReadings = 0;


    // 18. Turn off GSM.
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


  // 19. Turn on SD Card.
  // 20. Save data string to text file on SD card: "1399506809 1000 100" (roughly 20 characters)
  // -------------------------------------------------------------------------------------------

  // Try to turn on SD card. Should only need to be called once.
  snprintf(dataBuffer, TEMP_BUFFER_SIZE, "%d %d %d %d",
    totalReadings, unixTime, roundedTemperature, roundedDistance);

  sd.begin();
  sd.writeFile(BACKUP_FILENAME, dataBuffer);
  delay(1500);
}

double takeThermReading()
{
  // 2. Turn on thermistor.
  // ----------------------
  digitalWrite(THERM_PIN, HIGH);

  // 3. Take 5 thermistor readings. (one every 20ms)
  // -----------------------------------------------
  int thermReadings[NUM_THERM_READINGS];
  for (int i = 0; i < NUM_THERM_READINGS; ++i)
  {
    thermReadings[i] = analogRead(THERMISTOR_PIN);
    delay(THERM_READING_DELAY);
  }

  // 4. Turn off thermistor.
  // -----------------------
  digitalWrite(THERM_PIN, LOW);
  delay(500);

  // 5. Average 5 thermistor readings.
  // ---------------------------------
  double sumTherm = 0;
  for (int i = 0; i < NUM_THERM_READINGS; ++i)
  {
    sumTherm += thermReadings[i];
  }
  double avgTherm = sumTherm / NUM_THERM_READINGS;
  avgTherm = 1023 / avgTherm - 1;
  double R = 10000 / avgTherm;


  // 6. Convert average thermistor reading into temperature.
  // -------------------------------------------------------

  // Steinhart-Hart, modified:
  double avgTemperature = ( 3950.0 / (log( R / (10000.0 * exp( -3950.0 / 298.13 ) ) ) ) ) - 273.13;

  return avgTemperature;
}

double takeDistanceReading(double temperature)
{
  // 7. Turn on ultrasonic sensor (MOSFET).
  // --------------------------------------
  digitalWrite(MOSFET_US_PIN, HIGH);
  // Calibration time
  delay(3000);


  // 8. Take 3 distance readings. (One every 200ms)
  // ----------------------------------------------
  int distanceReadings[NUM_DISTANCE_READINGS];
  for (int i = 0; i < NUM_DISTANCE_READINGS; ++i)
  {
    distanceReadings[i] = analogRead(ULTRASONIC_PIN) * DISTANCE_INCREMENT;
    delay(DISTANCE_READING_DELAY);
  }


  // 9. Turn off ultrasonic sensor (MOSFET).
  // ---------------------------------------
  digitalWrite(MOSFET_US_PIN, LOW);
  delay(500);


  // 10. Average 3 distance measurements.
  // ------------------------------------
  double sumDistance = 0.0;
  for (int i = 0; i < NUM_DISTANCE_READINGS; ++i)
  {
    sumDistance += distanceReadings[i];
  }
  double avgDistance = sumDistance / NUM_DISTANCE_READINGS;


  // 11. Use average temperature to calculate actual distance.
  // ---------------------------------------------------------
  double adjustedDistance = ( ( 331.1 + .6 * temperature ) / 344.5 ) * avgDistance;
  if (SENSOR_TO_RIVER_BED > 0)
  {
    adjustedDistance = SENSOR_TO_RIVER_BED - adjustedDistance;
  }

  return adjustedDistance;
}