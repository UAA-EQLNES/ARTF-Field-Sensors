*Note: Documentation is a work in progress.*

## How to install the Arduino libraries for the bridge sensor

- Option 1: Drag the folders into the Arduino user libraries folder
- Option 2: Import each folders using the Arduino IDE's import library feature

## List of example sketches

The following are descriptions of the example sketches and their purpose:

#### UA_Sensors

- **BridgeSensorGSM** - Full sketch that utilizes all features of the sensor platform
- **BasicUltrasonic** - Used to verify readings from Ultrasonic senor
- **LowPowerGSMTest** - Verifies that the GSM shield sends on the sensor platform
- **LowPowerSDTest** - Verifies that the SD breakoutboard records data on the sensor platform
- **LowPowerThermistorTest** - Verifies that the Thermistor works on the sensor platform
- **LowPowerUltrasonicTest** - Verifies that the Ultrasonic sensor works on the sensor platform
- **Sim900CmdConsole** - A simple serial console to send AT commands to GSM shield

#### UA\_Sensors\_RTC

- **RTCTester** - Read/Set time on RTC board

#### UA\_Sensors\_SDCard

- **SDCardTester** - Read/Write text to SD Card

#### UA\_Sensors\_Sim900

- **Sim900Tester** - Verifies that the power functions and SMS messaging works