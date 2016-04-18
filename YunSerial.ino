#include <ProgmemString.h>
#include <Logging.h>
bool linuxBusy;

const byte YUN_HANDSHAKE_PIN = 7;
const long SERIAL_BAUD = 57600;
const int LOGGER_LEVEL = LOG_LEVEL_DEBUG;



//////////////////////////////////////////////////////////////////////////
// Arduino Functions
//////////////////////////////////////////////////////////////////////////

/**
* Startup code - Run once
*/
void setup()
{
	startYunSerial();
	Log.Info(P("Yun Serial - Handshake tester"));
}

/**
* Main loop - run forever
*/
void loop()
{
	Log.Info(P("Arduino handshake - Active"));
	delay(1000);
}

//////////////////////////////////////////////////////////////////////////
// Yun Serial

/**
* Set up the Arduino-Linux serial bridge
* Serial output from the Arduino is disabled until the Yun has finished booting
*/
void startYunSerial(){

	// Set up the handshake pin
	pinMode(YUN_HANDSHAKE_PIN, INPUT_PULLUP);
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

	// Give a delay for the AR9331 to reset, in case we were reset as part of a reboot command.
	// See http://playground.arduino.cc/Hardware/Yun#rebootStability, case 3.
	delay(5000);

	// If the system is booting, wait for UBoot to finish (Shamelessly adapted from Bridge.cpp)
	Serial1.begin(250000);
	do {
		while (Serial1.available() > 0) {
			Serial1.read();
		}
		delay(5000);
	} while (Serial1.available()>0);
	Serial1.end();

	// Check the initial state of the handshake pin (LOW == Ready)
	_bootStatusChange();

	Serial1.begin(SERIAL_BAUD);

	// Listen on the handshake pin for any changes
	attachInterrupt(4, _bootStatusChange, CHANGE);
}

/**
* Check the boot status of the Yun
*/
void _bootStatusChange(){
	linuxBusy = digitalRead(YUN_HANDSHAKE_PIN);
	digitalWrite(LED_BUILTIN, linuxBusy);

	// Disable log output until Linux boots
	if (linuxBusy){
		Log.Init(LOG_LEVEL_NOOUTPUT, &Serial1);
	}
	else{
		Log.Init(LOGGER_LEVEL, &Serial1);
	}
}
