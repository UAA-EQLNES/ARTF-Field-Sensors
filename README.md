# ARTF Field Sensor Sketches

This directory contains example and test sketches for the ARTF sensor
platform. Due to how the Ardino 1.0.x IDE works, the code is separated into
four separate libraries, three of which provide basic functionality for
the RTC clock, SD card, and GSM shield used on the platform.

## 1. Installation instructions

The sketches were tested with the Arduino 1.0.5 IDE.

**Option 1**

1. Download or clone the repository.
2. Drag the four folders into the Arduino user libraries folder
3. Open or restart Arduino IDE

**Option 2**

1. Download or clone the repository
2. Open the Arduino IDE
3. Import the four folders using the Arduino IDE's import library feature

### 1.1 Dependencies

- [Low Power](https://github.com/rocketscream/Low-Power)
- [Time](http://playground.arduino.cc/uploads/Code/Time.zip)
- [SDFat](https://code.google.com/p/sdfatlib/)

## 2. Description of sketches

The following are descriptions of the example sketches and their usage:

### 2.1 ARTF\_RTC

**RTCTester**

Reads and sets time on RTC breakout board. This sketch uses the console and
should be run without the sensor platform.
### 2.2 ARTF\_SDCard

**SDCardTester**

Reads and write text to SD breakout board. TThis sketch uses the console and
should be run without the sensor platform.

### 2.3 ARTF\_Sim900

**Sim900Tester**

Verifies that the power functions and text messaging works. This sketch uses the
console and should be run without the sensor platform.


### 2.4 ARTF\_Sensors

This section breaks up sketches into three sections: Tests, Examples, and
Utilities

#### 2.4.1 Test Sketches

These sketches were designed to test the individual functionality of the sensor
platform. It is a good idea to run each of the sketches in order to manually
verify that components are behaving correctly.

**BasicUltrasonic**

Used to verify that the Ultrasonic sensor works. This sketch continuously
takes 5 readings and calculates the sum and average. By default, readings
are multiplied by 5 to analog reading convert to millimeters. This is sensor
specific. The type used in the example is accurate up to millimeters and has a
maximum distance of 5 meters. The multiplier is required to fit within the
1024 bit limitation.

**BasicMoisture**

Used to verify that the moisture sensor works. This sketch continuously
takes 5 readings and calculates the sum and average. By default, the maximum
reading is set to 930.

This is because the sensor platform uses 3.3v "Arduinos" and thus the values of
the analog pins are spread over 3.3v. The `VH400 - Vegetronix Moisture Sensor Probe`
used for testing sends out 3v, where a full 0v is no moisture at all and 3v
would be submerging the whole thing in water.

The maximum analog reading, can be calculated like this:

`3v / 3.3v * 1024 = 930.91`

For 5v:

`3v / 5v * 1024 = 614.4`

**LowPowerThermistorTest**

Verifies that the thermistor works on the sensor platform.

This sketch will blink n + 1 times for each wake up.

If no blinks are seen or only a single blink, then the board
is reseting or not reading correctly.

You should detach the SD Card and RTC breakout boards as well
as the GSM shield. The sketch needs to be run from the battery.

**LowPowerUltrasonicTest**

This sketch specifically tests the ultrasonic sensor used
on our sensor platform.

This sketch will blink n + 1 times for each wake up.

If no blinks are seen or only a single blink, then the board
is reseting or not reading correctly.

You should detach the SD Card and RTC breakout boards as well
as the GSM shield. The sketch needs to be run from the battery.

There is a MOSFET controlled by pin **5** that needs to be
turned high for the ultrasonic sensor to take valid readings.

**LowPowerSDTest**

This sketch specifically tests the DeadOn RTC - DS3234 Breakout board
used on our sensor platform.

This sketch will write a value of n + 1 to the file test.txt each time
the RocketScream wakes up.

You should detach the RTC breakout board and GSM Shield.

**LowPowerUltrasonicWithSD**

This sketch is designed to test the accuracy of the Ultrasonic
sensor with the battery pack and circuit of the sensor platform.

This sketch takes 5 readings and averages them to
help verify similar calculations used in BridgeSensorGSM
sketch. The results are written to the SD card

**LowPowerGSMTest**

This sketch specifically tests the LinkSprite Sim900 GSM shield
used on our sensor platform.

This sketch will send a text message saying "Hello World " + n,
where n is the number of wake ups counted.

You should detach the SD Card and RTC breakout boards. The sketch
needs to be run from the battery.

#### 2.4.2 Examples

**BridgeSensorGSM**

This is an example sketch that demonstrates the following:

1. Take thermistor readings
2. Take ultrasonic sensor readings
3. Use thermistor readings to adjust ultrasonic readings
4. Retrieve time from RTC clock
5. Save data to SD card
6. Send text message at regular intervals

**MoistureSensorGSM**

This is an example sketch that demonstrates the following:

1. Take moisture readings
2. Retrieve time from RTC clock
3. Save data to SD card
4. Send text message at regular intervals

#### 2.4.3 Utilities

**Sim900CmdConsole**

This sketch allows you to send AT commands to the GSM shield via the console.

This sketch runs without the sensor platform and relies on the console for
input. A 5v "Arduino" is preferable since software serial is used. At 3.3v,
the default 19200 baudrate is too fast and makes the shield unresponsive to
commands.
