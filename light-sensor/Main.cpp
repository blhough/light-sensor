
const int PC_PIN = A0;	// PhotoCell		AIN
const int RF1_PIN = A1;	// RangeFinder 1	AIN
const int RF2_PIN = A2; // RangeFinder 2	AIN
const int LIT_PIN = 17;	// Relay Enable 1	DOUT	Light	NO
const int USB_PIN = 18; // Relay Enable 2	DOUT	USB		NC

const int TIMEOUT = 60 * 1000;

elapsedMillis timer;

bool isMotionDetected();
bool isRoomDark();
void turnLightOn();
void turnLightOff();

bool isLightOn = false;



int setup()
{
	pinMode( LIT_PIN, OUTPUT );
	pinMode( USB_PIN, OUTPUT );

	digitalWrite( LIT_PIN, LOW );
	digitalWrite( USB_PIN, LOW );
}

int loop()
{
	if( isMotionDetected() )
	{
		// Reset the timer anytime motion is detected.
		timer = 0;

		turnLightOn();
	}

	if( timer >= TIMEOUT )
	{
		// Hold timer steady so it does not overflow.
		timer = TIMEOUT;

		turnLightOff();
	}
}

bool isMotionDetected()
{
	return false;
}

bool isRoomDark()
{
	return false;
}

void turnLightOn()
{
	// If the light is off turn it on.
	if( !isLightOn )
	{
		isLightOn = true;
		digitalWrite( LIT_PIN, HIGH );
	}
}

void turnLightOff()
{
	// If the light is on turn it off.
	if( isLightOn )
	{
		isLightOn = false;
		digitalWrite( LIT_PIN, LOW );
	}
}
