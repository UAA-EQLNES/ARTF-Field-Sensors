/*
  Test Sim900 GSM Shield

  Test functionality needed for UA sensors platform, mainly,
  sending text messages and power toggle.

  Notes:
  - Make sure to change the phone number to real one!

  Created 14 6 2014
  Modified 22 6 2014
*/
#include <SoftwareSerial.h>
#include <ARTF_Sim900.h>

// UA Sensors Sim900 - Dependencies
#include <String.h>

// A test message for testing SMS functionality.
#define TEST_MSG "This is a test message from Sim900 GSM shield."

// Recipient phone number
#define PHONE_NUMBER "+12223334444"


// Instantiate Sim900 object. Need to call begin before using.
ARTF_Sim900 sim900;

// GSM Shield uses software serial on pins 7, 8. Need to
// switch jumpers and do some soldering to enable software serial.
//
// NOTE: Software serial does not seem to work with this shield using
// 8 Mhz Arduinos
//
SoftwareSerial Sim900Serial(7, 8);


// Menu options recognized from Serial input
const char SEND_TEXT_MSG = 's';
const char POWER_STATUS =  'i';
const char SHOW_MENU =     'm';
const char TOGGLE_POWER =  'p';
const char POWER_READY =   'r';
const char POWER_OFFLINE = 'o';


void setup()
{
  Serial.begin(19200);
  Sim900Serial.begin(19200);

  // Show menu
  doShowMenu();

  // Start up sim900 library with working serial connection to Sim900 shield
  sim900.begin(&Sim900Serial);
}


void loop()
{
  // Listen for user input. Perform action
  // if valid command specified from Serial.
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case SHOW_MENU:
        doShowMenu();
        break;
      case POWER_STATUS:
        doShowPowerStatus();
        break;
      case TOGGLE_POWER:
        doTogglePower();
        break;
      case POWER_READY:
        doPowerReady();
        break;
      case POWER_OFFLINE:
        doPowerOffline();
        break;
      case SEND_TEXT_MSG:
        doSendTextMsg();
        break;
    }
  }

  // Listen for input from GSM shield.
  if (sim900.available())
  {
    Serial.write(sim900.read());
  }
}

void doShowMenu()
{
  Serial.println("Sim900 Tester commands");
  Serial.println("-----------------------");
  Serial.println("m: Show menu");
  Serial.println("i: Check power status");
  Serial.println("s: Send text message");
  Serial.println("p: Toggle power on shield");
  Serial.println("r: Set shield to ready state (powered on)");
  Serial.println("o: Set shield to offline state");
  Serial.println();
}

// Check the power state stored in software. This is not
// the real hardware power state unfortunately, just a best
// guess based on status message from the GSM.
void doShowPowerStatus()
{
  Serial.print("GSM power status is...");
  if (sim900.isReady())
  {
    Serial.println("READY!\n");
  }
  else if (sim900.isOffline())
  {
    Serial.println("OFFLINE.\n");
  }
  else if (sim900.isUnknownState())
  {
    Serial.println("UKNOWN...Try toggling power in console to resync state");
  }
}

// Not recommended to use this functionality since
// power state is toggled and in most cases we need
// the exact state.
void doTogglePower()
{
  Serial.println("Toggling power on GSM shield...");

  sim900.togglePower();
  sim900.waitPowerToggleCompleted();

  if (sim900.isReady())
  {
    Serial.println("GSM shield powered on and ready.");
  }
  else if (sim900.isOffline())
  {
    Serial.println("GSM shield powered down successfully.");
  }
  else if (sim900.isUnknownState())
  {
    showErrorMessageForPower();
  }
}

// Tests functionality that ensures that the GSM is powered on
// We want to make sure the GSM is on and ready so we can
// send sensor readings via SMS.
void doPowerReady()
{
  Serial.println("Ensuring GSM shield is in READY state...");

  if (sim900.ensureReady())
  {
    Serial.println("GSM shield powered on and ready.");
  }
  else
  {
    showErrorMessageForPower();
  }
}

// Tests functionality that ensures that the GSM is powered off
// This is important to know otherwise battery could drain quickly.
void doPowerOffline()
{
  Serial.println("Ensuring GSM shield is in OFFLINE state...");

  if (sim900.ensureOffline())
  {
    Serial.println("GSM shield powered down successfully.");
  }
  else
  {
    showErrorMessageForPower();
  }
}

void doSendTextMsg()
{
  // Check if GSM is powered on and connected to network.
  // If not just quit witout trying.
  if (sim900.isOffline())
  {
    Serial.println("Error: Can't send text message if powered off!\n");
    return;
  }

  Serial.println("Sending a test message via SMS...\n");

  // Send test message to specified phone number. Both values
  // can be set at the top of the file.
  sim900.sendTextMsg(TEST_MSG, PHONE_NUMBER);

  // Check for shield response to see if we succeeded.
  if (sim900.isTextMsgDelivered() == true)
  {
    Serial.println("SMS text message sent successfully!\n");
  }
  else
  {
    Serial.println("Error: Could not send text message!\n");
  }
}

// General troubleshooting error message
void showErrorMessageForPower()
{
    Serial.println("Error: GSM shield state unknown. Possible reasons:");
    Serial.println("- Shield has no power source (check for green light)");
    Serial.println("- If using power from Arduino, make sure set to xduino");
    Serial.println("- Incorrect baudrates for serial console or shield");
    Serial.println("- Invalid sim card");
    Serial.println("- Timeout value too low");
}
