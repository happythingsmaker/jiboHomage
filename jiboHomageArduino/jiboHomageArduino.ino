// http://HappyThingsMaker.com


#include "FastLED.h"
#include "SmoothServoMotor.h"
#include "Queue.h"

#define DATA_PIN 2
#define NUM_LEDS 24

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);
CRGB leds[NUM_LEDS];

//Declare Spectrum Shield pin connections
#define STROBE 4
#define RESET 5
#define DC_One A0
#define DC_Two A1
//Define spectrum variables
#define NUM_SPECTRUM_CHANNEL	7
uint16_t frequenciesOnePrev[NUM_SPECTRUM_CHANNEL];
uint16_t frequenciesTwoPrev[NUM_SPECTRUM_CHANNEL];

uint16_t frequenciesOne[NUM_SPECTRUM_CHANNEL];
uint16_t frequenciesTwo[NUM_SPECTRUM_CHANNEL];

#define NUM_SAMPLES	5
#define NUM_LONG_SAMPLES	250

Queue<uint16_t> queue(NUM_SAMPLES);
Queue<uint16_t> longQueue(NUM_LONG_SAMPLES);

// Declare Servo
#define NUM_OF_SERVOS 3

#define PIN_SERVO_HEAD	9
#define PIN_SERVO_BODY	10
#define PIN_SERVO_FOOT	11

#define HEAD	0
#define BODY	1
#define FOOT	2

SmoothServoMotor servo[NUM_OF_SERVOS];

// Define thread time
#define UNIT_MS 10  // action every 10 ms
//#define UNIT_NEOPIXEL_MS 10
#define UNIT_NEOPIXEL_MS 10

void setup() {
	setupSpectrumShield();

	Serial.begin(9600);
	servo[HEAD].setup(PIN_SERVO_HEAD);
	servo[BODY].setup(PIN_SERVO_BODY);
	servo[FOOT].setup(PIN_SERVO_FOOT);

	//servo[HEAD].attach();

	Serial.println("Initialization complete");

	for (int i = 0; i < 10; i++) {

		queue.push(10);
	}

	//strip.begin();
	FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

}

uint32_t currentMillis;
long lastMillis;
long lastFrequencyMillis;
long lastADCMillis;
long lastNeopixelMillis;
uint16_t uSeconds = 100;
bool flag = false;
uint16_t prevAverage = 0;
uint8_t neopixelCount = 0;
uint16_t maxAverageTemp = 0;
uint16_t tempVolumToNeopixel;
uint16_t thresholdAverage = 0;
bool prevState = false;

void loop() {
	//rainbowCycle(10);
	while (1) {
		currentMillis = millis();

		// neopixel
		if (currentMillis - lastNeopixelMillis >= UNIT_NEOPIXEL_MS) {
			lastNeopixelMillis = currentMillis;

//			for (uint8_t i = 0; i < strip.numPixels(); i++) {
//				strip.setPixelColor(i,
//						Wheel(
//								((i * 256 / strip.numPixels()) + neopixelCount)
//										& 255));
//			}

			//strip.setBrightness(100);

//			maxAverageTemp = max(maxAverageTemp, highQueue.average());
//			tempVolumToNeopixel = map(maxAverageTemp, 0, 1023, 0, 12);
//
//			for (int i = NUM_PIXELS / 2; i > tempVolumToNeopixel; i--) {
//				strip.setPixelColor(i, strip.Color(5, 5, 5));
//				strip.setPixelColor(24 - i, strip.Color(5, 5, 5));
//			}

			//strip.show();
			leds[0] = CRGB::Red;
			if(!isAnyMotorRunning()){
			FastLED.show();
			}

//			if (neopixelCount++ >= 256 * 5) {
//				neopixelCount = 0;
//
//			}
			maxAverageTemp -= 10;

		}

		// motor
		if (currentMillis - lastMillis >= UNIT_MS) {
			lastMillis = currentMillis;
			//servoDriver.unitUpdate();
			//servo[HEAD].goAbsolutePosition()
			servo[HEAD].update();
			servo[BODY].update();
			servo[FOOT].update();
		}

		//if (currentMillis - lastADCMillis >= 2 && currentMillis - lastFrequencyMillis > 400) {
		//if (currentMillis - lastADCMillis >= 2 && !isAnyMotorRunning()) {
		if (currentMillis - lastADCMillis >= 0) {

			lastADCMillis = currentMillis;
			Read_Frequencies();

			Serial.print(queue.average());
			Serial.print(" ");
			Serial.println(longQueue.average() * 2);

//			if ((queue.average() > THRESHOLD_LEVEL)
//					&& (queue.average() - prevAverage) > 10) {
//
			thresholdAverage = longQueue.average() * 2;
			if ((queue.average() > thresholdAverage)
					&& (queue.average() > prevAverage)
					&& !isAnyMotorRunning()) {
				lastFrequencyMillis = currentMillis;

				if (prevState == false) {
					Serial.println("Boom");

					//				Serial.print(queue.average());
					//				Serial.print(" ");
					//				Serial.println(queue.average() - prevAverage);

					//				servo[HEAD].run(random(60, 120), 300);
					//				servo[BODY].run(random(60, 120), 300);
					//

					if (flag) {
						servo[HEAD].run(80 + random(0, 10), 300);
						servo[BODY].run(100 + random(0, 10), 300);
						flag = false;
					} else {
						servo[HEAD].run(100 + random(0, 10), 300);
						servo[BODY].run(80 + random(0, 10), 300);
						flag = true;
					}

					//				servo[HEAD].run(random(60, 120),
					//						max(300,
					//								min(
					//										(currentMillis - lastFrequencyMillis)
					//												* 2 / 3, 1000)));
					//				servo[BODY].run(random(60, 120),
					//						max(300,
					//								min(
					//										(currentMillis - lastFrequencyMillis)
					//												* 2 / 3, 1000)));

				}

				prevState = true;
			} else {
				prevState = false;

			}
		}

		if (Serial.available()) {
			uint8_t temp = Serial.read();
			if (temp == 'a') {
				servo[BODY].run(150, 1500);

			} else if (temp == 'b') {
				servo[BODY].run(45, 1500);

			} else if (temp == '1') {
				servo[HEAD].run(90, 500);
				servo[BODY].run(0, 500);

			} else if (temp == '2') {
				servo[HEAD].run(0, 500);
				servo[BODY].run(90, 500);

			} else if (temp == '3') {
				servo[HEAD].run(90, 500);
				servo[BODY].run(180, 500);

			} else if (temp == '4') {
				servo[HEAD].run(180, 500);
				servo[BODY].run(90, 500);

			} else if (temp == 'f') {

				servo[HEAD].run(90, 1000);
				servo[BODY].run(90, 1000);
				servo[FOOT].run(90, 1000);

			} else if (temp == 'n') {

				servo[HEAD].nod(30, 2000);

			}

			Serial.println(uSeconds);
		}

	}
}

/*******************Pull frquencies from Spectrum Shield********************/
void Read_Frequencies() {

	//Read frequencies for each band
	for (uint8_t freq_amp = 0; freq_amp < 7; freq_amp++) {
		frequenciesOnePrev[freq_amp] = frequenciesOne[freq_amp];
		frequenciesTwoPrev[freq_amp] = frequenciesTwo[freq_amp];
		frequenciesOne[freq_amp] = analogRead(DC_One);
		frequenciesTwo[freq_amp] = analogRead(DC_Two);
		digitalWrite(STROBE, HIGH);
		digitalWrite(STROBE, LOW);
	}

	prevAverage = queue.average();
	queue.push(frequenciesOne[0]);
	longQueue.push(frequenciesOne[0]);

}

void setupSpectrumShield() {
	//Set spectrum Shield pin configurations
	pinMode(STROBE, OUTPUT);
	pinMode(RESET, OUTPUT);
	pinMode(DC_One, INPUT);
	pinMode(DC_Two, INPUT);
	digitalWrite(STROBE, HIGH);
	digitalWrite(RESET, HIGH);

	//Initialize Spectrum Analyzers
	digitalWrite(STROBE, LOW);
	delay(1);
	digitalWrite(RESET, HIGH);
	delay(1);
	digitalWrite(STROBE, HIGH);
	delay(1);
	digitalWrite(STROBE, LOW);
	delay(1);
	digitalWrite(RESET, LOW);
}
bool isAnyMotorRunning() {
	return servo[HEAD].isRunning() || servo[BODY].isRunning()
			|| servo[FOOT].isRunning();
}

/*
void rainbowCycle(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j < 256 * 5; j++) {
		for (i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i,
					Wheel(((i * 256 / strip.numPixels()) + j) & 255));
		}
		strip.show();
		delay(wait);
	}
}

uint32_t Wheel(byte WheelPos) {
	if (WheelPos < 85) {
		return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	} else if (WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	} else {
		WheelPos -= 170;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
}
*/

