// http://HappyThingsMaker.com

#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "SmoothServoMotor.h"

#define NEO_PIN 13
#define NUM_PIXELS  24
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, NEO_PIN , NEO_GRB + NEO_KHZ800);

//Declare Spectrum Shield pin connections
#define STROBE 4
#define RESET 5
#define DC_One A0
#define DC_Two A1

// Declare Servo Driver
#define NUM_OF_SERVOS 9
#define PIN_SERVO_HEAD	9
#define PIN_SERVO_BODY	10
#define PIN_SERVO_FOOT	11

#define HEAD	0
#define BODY	1
#define FOOT	2


SmoothServoMotor servo[NUM_OF_SERVOS];

// Define thread time
#define UNIT_MS 10  // action every 10 ms

void setup() {
  Serial.begin(9600);
  servo[HEAD].setup(PIN_SERVO_HEAD);
  servo[BODY].setup(PIN_SERVO_BODY);
  servo[FOOT].setup(PIN_SERVO_FOOT);
  
  servo[HEAD].attach();
  
  Serial.println("Initialization complete");
}

long currentMillis;
long lastMillis;

uint16_t uSeconds = 100;

void loop() {
	rainbowCycle(10);
  while (1) {
    currentMillis = millis();
    if (currentMillis - lastMillis > UNIT_MS) {
      lastMillis  = currentMillis;
      //servoDriver.unitUpdate();
	  //servo[HEAD].goAbsolutePosition()
	  servo[HEAD].update();
    }
	
	if(Serial.available()){
		uint8_t temp = Serial.read() ;
		if(temp == 'a'){
			servo[HEAD].run(150, 1000);
			
		} else if(temp == 'b'){
			servo[HEAD].run(45, 1000);
			
		}
		else if(temp == '1'){
			
			servo[HEAD].run(0, 500);
			
		}
		else if(temp == '2'){
		
			servo[HEAD].run(180, 3000);
			
		}
		
		
		Serial.println(uSeconds);
	}
	
  }
}

void demo(int delayTime) {
  for (int i = 0 ; i < 90 ; i ++) {
    //servoDriver.go(0, 90 + i);
	//servoDriver.go(1, 90 - i);
	//servoDriver.go(2, 90 + i);
   
    delay(delayTime);
  }
  for (int i = 90 ; i > 0 ; i --) {
    //servoDriver.go(0, 90 + i);
	//servoDriver.go(1, 90 - i);
	//servoDriver.go(2, 90 + i);
    delay(delayTime);
  }
  
  for (int i = 0 ; i < 90 ; i ++) {
    //servoDriver.go(0, 90 - i);
	//servoDriver.go(1, 90 + i);
	//servoDriver.go(2, 90 - i);
   
    delay(delayTime);
  }
  for (int i = 90 ; i > 0 ; i --) {
   // servoDriver.go(0, 90 - i);
	//servoDriver.go(1, 90 + i);
	//servoDriver.go(2, 90 - i);
    delay(delayTime);
  }
  
  delay(100);
}


//NeoPixel에 달린 LED를 각각 다른색으로 시작하여 다양한색으로 5번 반복한다
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


//255가지의 색을 나타내는 함수
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
/*******************Pull frquencies from Spectrum Shield********************/
void Read_Frequencies() {
  int freq_amp;
  //Define spectrum variables
  int Frequencies_One[7];
  int Frequencies_Two[7];
  int i;

  //Read frequencies for each band

  for (freq_amp = 0; freq_amp < 7; freq_amp++)
  {
    Frequencies_One[freq_amp] = analogRead(DC_One);
    Frequencies_Two[freq_amp] = analogRead(DC_Two);
    digitalWrite(STROBE, HIGH);
    digitalWrite(STROBE, LOW);
  }

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
