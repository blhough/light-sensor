
const int PC_PIN = A0;	// PhotoCell		AIN
const int RF1_PIN = A1;	// RangeFinder 1	AIN
const int RF2_PIN = A2; // RangeFinder 2	AIN
const int LIT_PIN = 17;	// Relay Enable 1	DOUT	Light	NO
const int USB_PIN = 18; // Relay Enable 2	DOUT	USB		NC

const int TIMEOUT = 60 * 1000;
const int BRIGHTNESS_THRESHOLD = 100;
const int BRIGHTNESS_SAMPLES = 10;
const int DISTANCE_SAMPLES = 10;
const int DISTANCE_SENSITIVITY = 100;

elapsedMillis timer;

bool isMotionDetected();
bool isRoomDark();
void turnLightOn();
void turnLightOff();

bool isLightOn = false;
int avgBrightness = 0;
int avgDistance1 = 0;
int avgDistance2 = 0;



void setup()
{
	pinMode( LIT_PIN, OUTPUT );
	pinMode( USB_PIN, OUTPUT );

	digitalWrite( LIT_PIN, LOW );
	digitalWrite( USB_PIN, LOW );
}

void loop()
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
	// The voltage is not perfectly linear but close enough.
	// High voltage equates to short distance and vice versa but
	//  it does not matter since a |delta distance| will be used.
	int distance1 = analogRead( RF1_PIN );
	int distance2 = analogRead( RF2_PIN );

	int deltaDist1 = ( distance1 - avgDistance1 ) / DISTANCE_SAMPLES;
	int deltaDist2 = ( distance2 - avgDistance2 ) / DISTANCE_SAMPLES;

	// Keep a moving average of the distances.
	avgDistance1 += deltaDist1;
	avgDistance2 += deltaDist2;

	return abs( deltaDist1 ) > DISTANCE_SENSITIVITY || abs( deltaDist2 ) > DISTANCE_SENSITIVITY;
}

bool isRoomDark()
{
	int brightness = analogRead( PC_PIN );

	// Keep a moving average of the the room brightness.
	avgBrightness -= avgBrightness / BRIGHTNESS_SAMPLES;
	avgBrightness += brightness / BRIGHTNESS_SAMPLES;

	// If the average brightness is below the threshold needed to be considered dark.
	return avgBrightness < BRIGHTNESS_THRESHOLD;
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
