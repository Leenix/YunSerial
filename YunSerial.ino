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

	Serial1.begin(115200);

	// Check the initial state of the handshake pin (LOW == Ready)
	_bootStatusChange();

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
