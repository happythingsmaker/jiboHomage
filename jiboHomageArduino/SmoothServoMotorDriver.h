#ifndef _SMOOTHSERVOMOTORDRIVER_H_
#define _SMOOTHSERVOMOTORDRIVER_H_

#include <Adafruit_PWMServoDriver.h>

// following is library's ... almost same.
//#define MIN_PULSE_WIDTH       544     // the shortest pulse sent to a servo
//#define MAX_PULSE_WIDTH      2400     // the longest pulse sent to a servo
// Class Declaration ===========================================================
class SmoothServoMotorDriver {
	Adafruit_PWMServoDriver *pwmDriver;
	uint8_t pwmPin;
	uint16_t minValuePWM;
	uint16_t maxValuePWM;

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
		NORMAL, NOD, SUDDEN, REV_SUDDEN
	};

public:
	void setup(Adafruit_PWMServoDriver *argDriver, uint8_t argPin, uint16_t argMinValuePWM,
			uint16_t argMaxValuePWM) {
		pwmDriver = argDriver;
		pwmPin = argPin;
		minValuePWM = argMinValuePWM;
		maxValuePWM = argMaxValuePWM;

	}

	void attach() {
		//servo.attach(pwmPin); //, MIN_PULSE, MAX_PULSE);
	}

	void detach() {
		//servo.detach();
	}

	void moveStepAverageSpeed(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		int16_t thisTimeTarget = startLocation + (int32_t) currentStep * tempDiff / duration;

		//Serial.print("\t tempDiff : ");
		//Serial.print(tempDiff);
		//Serial.print("\t thisTimeTarget : ");
		//Serial.println(thisTimeTarget);
		//Serial.print("\t currentStep : ");
		//Serial.println(currentStep);

		//servo.write(thisTimeTarget);
		//servo.writeMicroseconds(thisTimeTarget);
		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;
	}

	void moveStepSin(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		//int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;
		double tempCalculation = 0.5 + (-1) * cos(PI * (double) currentStep / duration) / 2;
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;
		//Serial.println(thisTimeTarget);
		if (currentStep == duration) {
			thisTimeTarget = targetLocation;
		}

		/*
		 Serial.print("\t tempDiff : ");
		 Serial.print(tempDiff);
		 Serial.print("\t thisTimeTarget : ");
		 Serial.println(thisTimeTarget);
		 Serial.print("\t currentStep : ");
		 Serial.println(currentStep);
		 */
		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;

	}

	void moveStepSudden(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		//int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;
		double tempCalculation = sin(PI * (double) currentStep / (2 * duration));
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;
//		Serial.println(tempCalculation);
		if (currentStep == duration) {
			thisTimeTarget = targetLocation;
		}
		/*
		 Serial.print("\t tempDiff : ");
		 Serial.print(tempDiff);
		 Serial.print("\t thisTimeTarget : ");
		 Serial.println(thisTimeTarget);
		 Serial.print("\t currentStep : ");
		 Serial.println(currentStep);
		 */
		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;

	}
	void moveStepReverseSudden(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		//int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;
		double tempCalculation = 1 + (-1) * cos(PI * (double) currentStep / (2 * duration));
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;
				//Serial.println(tempCalculation);
		if (currentStep == duration) {
			thisTimeTarget = targetLocation;
		}
		/*
		 Serial.print("\t tempDiff : ");
		 Serial.print(tempDiff);
		 Serial.print("\t thisTimeTarget : ");
		 Serial.println(thisTimeTarget);
		 Serial.print("\t currentStep : ");
		 Serial.println(currentStep);
		 */
		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;

	}

	void moveStepNod(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		//int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;
		double tempCalculation = (1 + (-1) * cos(2 * PI * (double) currentStep / duration)) / 2;

		int16_t thisTimeTarget = startLocation + (double) tempDiff * tempCalculation;

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
		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
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
			} else if (operationFlag == SUDDEN) {
				moveStepSudden(innerCount);
			} else if (operationFlag == REV_SUDDEN) {
				moveStepReverseSudden(innerCount);
			}

			if (innerCount == duration) {
				runningFlag = false;
				toqueOff();
			}

			innerCount++;

		}

	}

	void run(int16_t argTargetLocation, uint16_t argDuration) {
		runningFlag = true;
		operationFlag = NORMAL;
		innerCount = 1;
		isFirst = true;
		duration = argDuration / 10;
		startLocation = currentLocation;
		targetLocation = argTargetLocation;
	}

	void nod(int16_t argStartLocation, int16_t argDiffAngle, uint16_t argDuration) {
		runningFlag = true;
		operationFlag = NOD;
		innerCount = 1;
		isFirst = true;
		duration = argDuration / 10;
		startLocation = argStartLocation;
		targetLocation = argStartLocation + argDiffAngle;
	}
	void sudden(int16_t argTargetLocation, uint16_t argDuration) {
		runningFlag = true;
		operationFlag = SUDDEN;
		innerCount = 1;
		isFirst = true;
		duration = argDuration / 10;
		startLocation = currentLocation;
		targetLocation = argTargetLocation;
	}
	void revSudden(int16_t argTargetLocation, uint16_t argDuration) {
		runningFlag = true;
		operationFlag = REV_SUDDEN;
		innerCount = 1;
		isFirst = true;
		duration = argDuration / 10;
		startLocation = currentLocation;
		targetLocation = argTargetLocation;
	}

	void stop() {
		runningFlag = false;
		innerCount = 0;
	}

	void toqueOff() {
		pwmDriver->setPWM(pwmPin, 0, 0);
	}

	bool isRunning() {
		return runningFlag;
	}

	void write(uint8_t position) {
		// it is from 0 to 180 degree
		pwmDriver->setPWM(pwmPin, 0, map(position, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = position;
		//@param  on At what point in the 4096-part cycle to turn the PWM output ON
		//@param  off At what point in the 4096-part cycle to turn the PWM output OFF

	}

};
#endif
