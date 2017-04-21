
const int PC_PIN = A0;	// PhotoCell		AIN
const int RF1_PIN = A1;	// RangeFinder 1	AIN
const int RF2_PIN = A5; // RangeFinder 2	AIN
const int LIT_PIN = 17;	// Relay Enable 1	DOUT	Light	NO
const int USB_PIN = 18; // Relay Enable 2	DOUT	USB		NC

const int TIMEOUT_WHILE_DARK = 5 * 1000;
const int TIMEOUT_WHILE_LIGHT = 3 * 1000;
const int TIMOUT_MAX = TIMEOUT_WHILE_DARK > TIMEOUT_WHILE_LIGHT ? TIMEOUT_WHILE_DARK : TIMEOUT_WHILE_LIGHT;
const int BRIGHTNESS_THRESHOLD = 100;
const float BRIGHTNESS_SAMPLES = 12;
const int DISTANCE_SAMPLES = 4;
const int DISTANCE_SENSITIVITY = 90;

const int ANALOG_RES = 12;

elapsedMillis timer;

bool isMotionDetected();
bool isRoomDark();
void turnLightOn();
void turnLightOff();

bool isLightOn = false;
float avgBrightness = 0;
int avgDistance1 = 0;
int avgDistance2 = 0;



void setup()
{
	Serial.begin(115200);
	analogReadResolution(ANALOG_RES);
	//analogReadAveraging(8);

	pinMode(LIT_PIN, OUTPUT);
	pinMode(USB_PIN, OUTPUT);

	pinMode(PC_PIN, INPUT);
	pinMode(RF1_PIN, INPUT);
	pinMode(RF2_PIN, INPUT);


	digitalWrite(LIT_PIN, HIGH);
	digitalWrite(USB_PIN, HIGH);
}

void loop()
{
	// Serial.print("B: ");
	//Serial.print(avgBrightness);
	//Serial.print(", ");
	Serial.print(avgDistance1);
	Serial.print(", ");
	Serial.println(avgDistance2);
	//Serial.print(", T: ");
	//Serial.println(timer);

	bool isDark = isRoomDark();
	bool isMotion = isMotionDetected();
	isDark = true;


	if (isDark && isMotion)
	{
		// Reset the timer anytime motion is detected.
		timer = 0;

		turnLightOn();
	}

	if ((isDark && timer >= TIMEOUT_WHILE_DARK) || (!isDark && timer >= TIMEOUT_WHILE_LIGHT))
	{
		// Hold timer steady so it does not overflow.
		timer = TIMOUT_MAX;
		turnLightOff();
	}
}

bool isMotionDetected()
{
	// The voltage is not perfectly linear but close enough.
	// High voltage equates to short distance and vice versa but
	//  it does not matter since a |delta distance| will be used.
	int distance1 = 0;
	int distance2 = 0;
	const int samples = 15;

	int distances1[samples];
	int distances2[samples];

	int max1 = 0;
	int max2 = 0;

	int min1 = pow(2, ANALOG_RES);
	int min2 = pow(2, ANALOG_RES);


	for (size_t i = 0; i < samples; i++)
	{
		distances1[i] = analogRead(RF1_PIN);
		delay(6);

		distances2[i] = analogRead(RF2_PIN);
		delay(6);
	}

	sort(0, samples, distances1);
	sort(0, samples, distances2);
	distance1 = distances1[samples / 2];
	distance2 = distances2[samples / 2];


	//Serial.println(distance1);

	int deltaDist1 = avgDistance1;
	int deltaDist2 = avgDistance2;

	float smoothing = 0.7f;

	// Keep a moving average of the distances.
	avgDistance1 = avgDistance1 * smoothing + (1 - smoothing) * distance1;
	avgDistance2 = avgDistance2 * smoothing + (1 - smoothing) * distance2;
	//Serial.println(avgDistance1);

	deltaDist1 -= avgDistance1;
	deltaDist2 -= avgDistance2;

	return abs(deltaDist1) > DISTANCE_SENSITIVITY || abs(deltaDist2) > DISTANCE_SENSITIVITY;
}

bool isRoomDark()
{
	int brightness = analogRead(PC_PIN);

	// Keep a moving average of the the room brightness.
	avgBrightness -= avgBrightness / BRIGHTNESS_SAMPLES;
	avgBrightness += brightness / BRIGHTNESS_SAMPLES;

	// If the average brightness is below the threshold needed to be considered dark.
	return avgBrightness < BRIGHTNESS_THRESHOLD;
}

void turnLightOn()
{
	// If the light is off turn it on.
	if (!isLightOn)
	{
		isLightOn = true;
		digitalWrite(LIT_PIN, LOW);
	}
}

void turnLightOff()
{
	// If the light is on turn it off.
	if (isLightOn)
	{
		isLightOn = false;
		digitalWrite(LIT_PIN, HIGH);
		for (size_t i = 0; i < 50; i++)
		{
			isMotionDetected();
		}
	}
}

void sort(unsigned a, unsigned b, int arr[])
{
	for (size_t i = a + 1; i < b; i++)
	{
		int val = arr[i];

		for (size_t j = i - 1; j >= a; j--)
		{
			if (arr[j] <= val)
			{
				arr[j + 1] = val;
				break;
			}

			arr[j + 1] = arr[j];
		}
	}
}