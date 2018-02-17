#ifndef _SMOOTHSERVOMOTOR_H_
#define _SMOOTHSERVOMOTOR_H_

#include <Servo.h>
/*

 #define MAX_MG90D 450
 #define MIN_MG90D 140
 #define FREQUENCY_MG90D 50

 #define MAX_MG90S 500
 #define MIN_MG90S 100
 #define FREQUENCY_MG90D 50

 // microsecond : max : 2400
 // microsecond : min: 550

 */

// following is library's ... almost same.
//#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo
//#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo
// Class Declaration ======================================================================
class SmoothServoMotor {
	Servo servo;

	uint8_t pwmPin;
	uint32_t innerCount = 0;
	uint32_t lastUpdate = 0;
	uint16_t duration = 1;

	int16_t startLocation = 0;
	int16_t currentLocation = 0;
	int16_t targetLocation = 0;

	bool runningFlag = false;
	bool isFirst;

	uint8_t operationFlag = 0;
	enum OPERATION {
		NORMAL, NOD
	};

	//const double PI = 3.14159265359;

public:
	void setup(uint8_t argPin) {
		pwmPin = argPin;
	}

	void attach() {
		servo.attach(pwmPin); //, MIN_PULSE, MAX_PULSE);
	}

	void detach() {
		servo.detach();
	}

	void moveStepAverageSpeed(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		int16_t thisTimeTarget = startLocation
				+ (int32_t) currentStep * tempDiff / duration;

		//Serial.print("\t tempDiff : ");
		//Serial.print(tempDiff);
		//Serial.print("\t thisTimeTarget : ");
		//Serial.println(thisTimeTarget);
		//Serial.print("\t currentStep : ");
		//Serial.println(currentStep);

		//servo.write(thisTimeTarget);
		servo.writeMicroseconds(thisTimeTarget);
		currentLocation = thisTimeTarget;
	}

	void moveStepSin(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		//int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;
		double tempCalculation = 0.5
				+ (-1) * cos(PI * (double) currentStep / duration) / 2;
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;
		//Serial.println(thisTimeTarget);
		if (currentStep == duration) {
			thisTimeTarget = targetLocation;
		}

		//Serial.print("\t tempDiff : ");
		//Serial.print(tempDiff);
		//Serial.print("\t thisTimeTarget : ");
		//Serial.println(thisTimeTarget);
		//Serial.print("\t currentStep : ");
		//Serial.println(currentStep);

		//servo.write(thisTimeTarget);
		servo.writeMicroseconds(thisTimeTarget);
		currentLocation = thisTimeTarget;
	}

	void moveStepNod(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		//int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;
		double tempCalculation = (1
				+ (-1) * cos(2 * PI * (double) currentStep / duration)) / 2;

		int16_t thisTimeTarget = startLocation
				+ (double) tempDiff * tempCalculation;


		//Serial.println(thisTimeTarget);
		if (currentStep == duration) {
			thisTimeTarget = startLocation;
		}

		//Serial.print("\t tempDiff : ");
		//Serial.print(tempDiff);
		//Serial.print("\t thisTimeTarget : ");
		//Serial.println(thisTimeTarget);
		//Serial.print("\t currentStep : ");
		//Serial.println(currentStep);

		//servo.write(thisTimeTarget);
		servo.writeMicroseconds(thisTimeTarget);
		currentLocation = thisTimeTarget;
	}

	void moveCurrentStep(int currentPower, int nextPower, int timeUnitCount) {

#ifdef DEBUG_MOTION
		if (pin == PIN_NUMBER_SERVO_LEFT) {
			Serial.print("[L]");
		} else if (pin == PIN_NUMBER_SERVO_RIGHT) {
			Serial.print("\t\t[R]");
		}
		Serial.print("[");
		Serial.print(timeUnitCount);
		Serial.print("] :");
#endif
		/*
		 int hitPosition;

		 double startLocation = hitPosition + hitDirection * currentPower * POSITION_MAXDIFF / 100;
		 double endLocation = hitPosition + hitDirection * nextPower * POSITION_MAXDIFF / 100;

		 if (timeUnitCount < (TIME_MS_DURATION_FOR_1_HIT / TIME_MS_CONTROL_FREQ_UNIT) / 2) {
		 servo.write(hitPosition);
		 } else if (timeUnitCount == (TIME_MS_DURATION_FOR_1_HIT / TIME_MS_CONTROL_FREQ_UNIT) / 2) {
		 servo.write(hitPosition);
		 } else {
		 servo.write(endLocation);
		 }
		 */
	}

	void update() {

		// time to update
		if (runningFlag) {

			if (isFirst) {
				attach();
				isFirst = false;
			}

			if (operationFlag == NORMAL) {
				moveStepSin(innerCount);
			} else if (operationFlag == NOD) {
				moveStepNod(innerCount);
			}

			if (innerCount == duration) {
				runningFlag = false;
				//detach();
			}

			innerCount++;

		}

	}

	void run(int16_t argTargetLocation, uint16_t argDuration) {
		runningFlag = true;
		operationFlag = NORMAL;
		innerCount = 0;
		isFirst = true;
		duration = argDuration / 10;
		startLocation = currentLocation;
		//targetLocation = argTargetLocation;
		targetLocation = map(argTargetLocation, 0, 180, MIN_PULSE_WIDTH,
				MAX_PULSE_WIDTH);
	}

	void nod(int16_t argTargetLocation, uint16_t argDuration) {
		runningFlag = true;
		operationFlag = NOD;
		innerCount = 0;
		isFirst = true;
		duration = argDuration / 10;
		startLocation = currentLocation;
		targetLocation = map(argTargetLocation, 0, 180, MIN_PULSE_WIDTH,
				MAX_PULSE_WIDTH);
	}

	void stop() {
		runningFlag = false;
		innerCount = 0;
	}

	bool isRunning() {
		return runningFlag;
	}

	void goAbsolutePosition(uint16_t position) {
		// it is from 0 to 180 degree
		servo.write(position);
	}

	void writeMicroseconds(uint16_t uSeconds) {
		// it is based on uSeconds. from 544 to 2400
		servo.writeMicroseconds(uSeconds);
	}

};
#endif
