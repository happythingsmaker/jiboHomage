#ifndef _SMOOTHSERVOMOTORDRIVER_H_
#define _SMOOTHSERVOMOTORDRIVER_H_

#include <Adafruit_PWMServoDriver.h>

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

	void moveStepAverageSpeed(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		int16_t thisTimeTarget = startLocation + (int32_t) currentStep * tempDiff / duration;

		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;
	}

	void moveStepSin(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		double tempCalculation = 0.5 + (-1) * cos(PI * (double) currentStep / duration) / 2;
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;

		if (currentStep == duration) {
			thisTimeTarget = targetLocation;
		}

		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;

	}

	void moveStepSudden(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		double tempCalculation = sin(PI * (double) currentStep / (2 * duration));
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;

		if (currentStep == duration) {
			thisTimeTarget = targetLocation;
		}

		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;

	}
	void moveStepReverseSudden(int16_t currentStep) {

		int16_t tempDiff = targetLocation - startLocation;
		double tempCalculation = 1 + (-1) * cos(PI * (double) currentStep / (2 * duration));
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;

		if (currentStep == duration) {
			thisTimeTarget = targetLocation;
		}

		Serial.println(thisTimeTarget);

		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;
	}

	void moveStepNod(int16_t currentStep) {
		int16_t tempDiff = targetLocation - startLocation;
		double tempCalculation = (1 + (-1) * cos(2 * PI * (double) currentStep / duration)) / 2;
		int16_t thisTimeTarget = startLocation + (double) tempDiff * tempCalculation;

		if (currentStep == duration) {
			thisTimeTarget = startLocation;
		}

		pwmDriver->setPWM(pwmPin, 0, map(thisTimeTarget, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = thisTimeTarget;
	}

	void update() {

		if (runningFlag) {
			if (isFirst) {
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
		//from 0 to 180 degree
		pwmDriver->setPWM(pwmPin, 0, map(position, 0, 180, minValuePWM, maxValuePWM));
		currentLocation = position;
	}

};
#endif
