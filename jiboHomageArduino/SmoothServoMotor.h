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
	
	bool isRunning = false;
	bool isFirst;
	
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
	
	void moveStepAverageSpeed(int16_t currentStep){
		// 등속도. 처음과 끝, 그리고 스텝에 따라서 올려주기만 하면 됨.
		int16_t tempDiff = targetLocation - startLocation ;
		int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;

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
	
	void moveStepSin(int16_t currentStep){
		// 등속도. 처음과 끝, 그리고 스텝에 따라서 올려주기만 하면 됨.
		int16_t tempDiff = targetLocation - startLocation ;
		//int16_t thisTimeTarget = startLocation + (int32_t)currentStep * tempDiff / duration;
		double tempCalculation = 0.5 + (-1) * cos(PI * (double)currentStep / duration) / 2;
		int16_t thisTimeTarget = startLocation + tempDiff * tempCalculation;
		Serial.println(thisTimeTarget);
		if (currentStep == duration){
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


	// 모션 프로파일링의 한 조각. 매 주기마다 실행되며 현재 주기는 10ms ========================================================
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

		// 모션 제어 시작 ============================================================================
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
		if (isRunning) {
			
			// 처음이었다면 모터 on
			if (isFirst) {
				attach();
				isFirst = false;
			}

			//moveStepAverageSpeed(innerCount);
			moveStepSin(innerCount);
			
			if (innerCount == duration) {
				isRunning = false;
			}

			innerCount++;
			
		
		}
		
	}

	

	void run(int16_t argTargetLocation, uint16_t argDuration) {
		isRunning = true;
		innerCount = 0;
		isFirst = true;
		duration = argDuration/10;
		startLocation = currentLocation;
		//targetLocation = argTargetLocation;
		targetLocation = map(argTargetLocation,0,180,MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
	}
	
	void stop() {
		isRunning = false;
		innerCount = 0;
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
