// http://HappyThingsMaker.com

#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_NeoPixel.h>

#include "SmoothServoMotorDriver.h"

#include "Queue.h"

#define DEBUG	0

#define NEO_PIN 2
#define NUM_PIXELS 24
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN, NEO_GRB + NEO_KHZ800);

//Declare Spectrum Shield pin connections ==========================================================
#define STROBE 4
#define RESET 5
#define DC_One A0
#define DC_Two A1

//Define spectrum variables ========================================================================
#define NUM_SPECTRUM_CHANNEL	7
int16_t frequenciesOne[NUM_SPECTRUM_CHANNEL];
int16_t frequenciesTwo[NUM_SPECTRUM_CHANNEL];

#define NUM_SAMPLES	5
//#define NUM_LONG_SAMPLES	100
#define NUM_LONG_SAMPLES	20
#define NUM_BPM_SAMPLES	5

Queue<int16_t> shortQueue(NUM_SAMPLES);
Queue<int16_t> longQueue(NUM_LONG_SAMPLES);
Queue<int16_t> bpmQueue(NUM_BPM_SAMPLES);
Queue<int16_t> neopixelQueue(10);

Queue<int16_t> resultQueue(100);

enum readingSectrumMode {
	NORMAL, MOTION_RUNNING
};

int16_t initialVolumeLevel = 0;

// Declare Servo ==================================================================================
#define MIN_MG90S 102	 //  4096 / 40 == 102.4 based on a general servo range
#define MAX_MG90S 512	// 4096 / 8 == 512  	based on a general servo range
#define FREQUENCY_MG90SS 51	// 50 Hz (needs to add 1)

#define NUM_OF_SERVOS 3

#define PIN_SERVO_HEAD	0
#define PIN_SERVO_BODY	1
#define PIN_SERVO_FOOT	2

#define HEAD	0
#define BODY	1
#define FOOT	2

Adafruit_PWMServoDriver pwmDriver = Adafruit_PWMServoDriver(0x40);
SmoothServoMotorDriver servo[NUM_OF_SERVOS];

// Define thread time ==============================================================================
#define UNIT_MS_NEOPIXEL 10
#define UNIT_NEOPIXEL_MS 10

// ================================================================================================
void setup() {

	pwmDriver.begin();
	servo[HEAD].setup(&pwmDriver, PIN_SERVO_HEAD, MIN_MG90S, MAX_MG90S);
	servo[BODY].setup(&pwmDriver, PIN_SERVO_BODY, MIN_MG90S, MAX_MG90S);
	servo[FOOT].setup(&pwmDriver, PIN_SERVO_FOOT, MIN_MG90S, MAX_MG90S);
	pwmDriver.setPWMFreq(51);  //

	setupSpectrumShield();

	strip.begin();
	Serial.begin(115200);

#if DEBUG
	Serial.println("Initialization complete");
#endif

}

// ================================================================================================

uint16_t currentMillis;
uint16_t lastMotorUpdateMillis;
uint16_t lastBeatMillis;
uint16_t lastSpectrumCheckMillis;
uint16_t lastNeopixelMillis;
bool flag = false;
uint16_t prevAverage = 0;
uint8_t neopixelCount = 0;
uint16_t maxAverageTemp = 0;
uint16_t tempVolumToNeopixel;
uint16_t thresholdAverage = 0;
bool prevState = false;

uint8_t tempFreqeuncy = 90;

uint8_t motionSet[8][3] = { { 90, 90, 90 }, { 90 - 10, 90 + 10, 90 }, { 90 - 20, 90 + 20, 90 }, { 90
		- 30, 90 + 30, 90 }, { 90 + 10, 90 + 10, 90 }, { 90 + 20, 90 + 20, 90 }, { 90 - 10, 90 - 10,
		90 }, { 90 - 20, 90 - 20, 90 },

};

enum STATE {
	LISTENING, IDLE
};
uint32_t lastBoomTime = 0;

int16_t tempTest = 0;
uint8_t ledRotationCount = 0;
uint8_t ledRotationStep = 0;
uint16_t quiteCount = 0;
uint8_t testVariable = 0;

void loop() {

	pinMode(8, INPUT_PULLUP);
	while (1) {
		if (digitalRead(8) == LOW) {
			if (testVariable) {
				testVariable = false;

			} else {
				testVariable = true;

			}
			delay(100);
		}

		currentMillis = millis();

		// motor control routine ===================================================================
		if (currentMillis - lastMotorUpdateMillis >= UNIT_MS_NEOPIXEL) {
			lastMotorUpdateMillis = currentMillis;
			servo[HEAD].update();
			servo[BODY].update();
			servo[FOOT].update();
		}

		// neopixel control routine ================================================================
		if (currentMillis - lastNeopixelMillis >= UNIT_NEOPIXEL_MS) {
			lastNeopixelMillis = currentMillis;

			for (uint8_t i = 0; i < strip.numPixels(); i++) {
				strip.setPixelColor(i,
						Wheel(((i * 256 / strip.numPixels()) + neopixelCount) & 255));
			}

			maxAverageTemp = max(maxAverageTemp, neopixelQueue.average());
			maxAverageTemp = max(100, min(maxAverageTemp, 512));

			/*
			 //spead from centre
			 tempVolumToNeopixel = map(maxAverageTemp, 100, 512, 0, 12);
			 for (uint8_t i = NUM_PIXELS / 2; i > tempVolumToNeopixel; i--) {
			 strip.setPixelColor(i, strip.Color(2, 2, 2));
			 strip.setPixelColor(24 - i, strip.Color(2, 2, 2));

			 }
			 */
			//spread from head to tail
			tempVolumToNeopixel = map(maxAverageTemp, 100, 512, 1, 24);

			for (uint8_t i = 0; i < NUM_PIXELS - tempVolumToNeopixel; i++) {
				strip.setPixelColor(i, strip.Color(2, 2, 2));
			}

			// 10 ms * 100 time == every 1 second
			if (ledRotationCount++ > 100) {
				ledRotationCount = 0;
				ledRotationStep++;

			}

			if (ledRotationStep >= 24) {
				ledRotationStep = 0;
			}

			//backup
			uint32_t tempColorSet[24] = { 0 };
			for (uint8_t i = 0; i < NUM_PIXELS; i++) {
				tempColorSet[i] = strip.getPixelColor(i);
			}

			// setting again
			for (uint8_t i = 0; i < NUM_PIXELS; i++) {
				//for (uint8_t i = NUM_PIXELS - 1; i >= 0; i--) {
				strip.setPixelColor(i,
						tempColorSet[(i + NUM_PIXELS - ledRotationStep) % NUM_PIXELS]);
			}

			if (maxAverageTemp < 120) {
				// Check slince time for 10 ms * 500 times (5 second)
				// if slience lasts 5 second, robot turns off the leds
				if (quiteCount++ > 500) {
					quiteCount = 501;
					for (uint8_t i = 0; i < strip.numPixels(); i++) {
						strip.setPixelColor(i, 0);
					}
				}
			} else {
				quiteCount = 0;

			}

			strip.show();

			if (neopixelCount++ >= 256 * 5) {
				neopixelCount = 0;

			}
			if (maxAverageTemp > 110) {
				maxAverageTemp -= 5;
			}

		}

		// sound spectrum analizing routine ========================================================
		// Spectrum Analizing Period : 2 ms per channel
		if (currentMillis - lastSpectrumCheckMillis >= 2) {

			lastSpectrumCheckMillis = currentMillis;
			bpmQueue.push(currentMillis - lastBeatMillis);

			// FIRED!
			if (Read_Frequencies(NORMAL)) {
				ledRotationStep += 4;

				int16_t tempAngle = random(30, 45);
				int16_t tempDuration = min(1500, bpmQueue.average());
				if (tempDuration == 1500) {
					tempDuration = 300;
				}

				if (testVariable) {
					if (flag) {
						flag = false;
						if (random(3)) {
							servo[HEAD].revSudden(90 + random(30, 60), tempDuration);
						}

						if (random(3)) {
							servo[BODY].revSudden(90 - random(30, 60), tempDuration);
						}

						if (random(3)) {
							servo[FOOT].revSudden(90 + random(-90, 90), tempDuration);
						}

					} else {
						if (random(3)) {
							servo[HEAD].revSudden(90 - random(30, 60), tempDuration);
						}

						if (random(3)) {
							servo[BODY].revSudden(90 + random(30, 60), tempDuration);
						}
						if (random(3)) {
							servo[FOOT].revSudden(90 + random(-90, 90), tempDuration);
						}
						flag = true;
					}
				} else {
					if (flag) {
						flag = false;
#define TEMPDGREE 60
						//servo[HEAD].sudden(90 - TEMPDGREE, tempDuration);
						servo[BODY].sudden(90 + TEMPDGREE, tempDuration);
						servo[FOOT].sudden(90 - TEMPDGREE, tempDuration);
					} else {
						//servo[HEAD].sudden(90 - TEMPDGREE, tempDuration);
						servo[BODY].sudden(90 - TEMPDGREE, tempDuration);
						servo[FOOT].sudden(90 + TEMPDGREE, tempDuration);
						flag = true;
					}


				}

				//servo[HEAD].nod(90, tempAngle, tempTime);
				//servo[BODY].nod(90, -tempAngle, tempTime);
//				servo[HEAD].sudden(90 + random(-40, 40), min(1000, bpmQueue.average() * 3 / 4));
//				servo[BODY].sudden(90 + random(-40, 40), min(1000, bpmQueue.average() * 3 / 4));
//				servo[FOOT].sudden(90 + random(-10, 10), min(1000, bpmQueue.average() * 3 / 4));

				lastBeatMillis = currentMillis;
				// in order to fire for motors EARLY
				lastMotorUpdateMillis = currentMillis + 100;

			}
		}

		if (Serial.available()) {
			uint8_t temp = Serial.read();
			if (temp == 'a') {
				servo[BODY].sudden(120, 500);

			} else if (temp == 'b') {
				servo[BODY].sudden(60, 300);

			} else if (temp == 'c') {
				servo[BODY].sudden(30, 1000);

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

				servo[HEAD].nod(90, 30, 300);

			} else if (temp == 'p') {

				resultQueue.push(tempTest += 2);
				resultQueue.printQueue();

			} else if (temp == 'm') {

				if (testVariable++ >= 7) {
					testVariable = 0;
				}

			}

		}

	}
}

/*******************Pull frquencies from Spectrum Shield********************/
uint8_t freq_amp = 0;
bool Read_Frequencies(uint8_t mode) {

	int16_t temp = 0;
	int16_t tempThreshold = 0;

	//Read frequencies for each band
	frequenciesOne[freq_amp++] = analogRead(DC_One);

	if (freq_amp >= 7) {
		freq_amp = 0;
	}

	// changing frequency channel by controlling STROBE
	digitalWrite(STROBE, HIGH);
	digitalWrite(STROBE, LOW);

	if (freq_amp == 0) {
		shortQueue.push(frequenciesOne[0]);
		longQueue.push(shortQueue.average());
		neopixelQueue.push(frequenciesOne[0] + frequenciesOne[2]);

		// true graph data
		temp = shortQueue.average() - longQueue.getMin();
		// putting that data into another queue
		resultQueue.push(temp);
		tempThreshold = max(25, resultQueue.getMax() / 2);

#if DEBUG
		Serial.print(temp);
		Serial.print(" ");
		Serial.println(tempThreshold);
#endif

	}

//	Serial.print(tempThreshold);
//	Serial.print(" ");
//	Serial.println(temp);
//	if (temp > 10) {
	if (temp > tempThreshold) {
		if (currentMillis - lastBoomTime > 300) {
			lastBoomTime = currentMillis;
			//Serial.println(temp);
			return true;

		} else {
			//Serial.println(0);
		}

	} else {
		//Serial.println(0);
	}
	return false;

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
	return servo[HEAD].isRunning() || servo[BODY].isRunning() || servo[FOOT].isRunning();
}

void rainbowCycle(uint8_t wait) {
	uint16_t i, j;

	for (j = 0; j < 256 * 5; j++) {
		for (i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
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

void groove() {

	if (flag) {
		servo[HEAD].run(80 + (0, 30), min(1000, bpmQueue.average() / 2));
		servo[BODY].run(100 + random(0, 30), min(1000, bpmQueue.average() / 2));
		flag = false;
	} else {
		servo[HEAD].run(100 + random(0, 30), min(1000, bpmQueue.average() / 2));
		servo[BODY].run(80 + random(0, 30), min(1000, bpmQueue.average() / 2));
		flag = true;
	}

}
