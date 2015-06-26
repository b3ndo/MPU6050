// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _M6050comp_H_
#define _M6050comp_H_
#include "Arduino.h"
//add your includes for the project M6050 here
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Wire.h>
#include <HMC5883L.h>
#include <MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Buzzer.h>

//end of add your includes here
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

int buttonPin = 8;
int battAin = A0; int battBin = A1;
int interval = 5; int iz = 0;
float battA; float battB;
float tempBMP;
float tempMPU;
float pressure;
float altitude;
float heading;
float headingDegrees;
float calculseaLevelPressure;
float seaLevelPressure = 1013.25;	//Standardny tlak na hladine mora
//		float seaLevelPressure = 1013.25; 	//Nadmorska vyska

// Set declination angle on your location and fix heading
// You can find your declination on: http://magnetic-declination.com/
// (+) Positive or (-) for negative
// For Bytom / Poland declination angle is 4'26E (positive)
// Formula: (deg + (min / 60.0)) / (180 / M_PI);
float declinationAngle = (4.0 + (34.0 / 60.0)) / (180 / M_PI);

//add your function definitions for the project M6050comp here
void zobraz(); 	//Zobrazi na LCD
void zapis();	//Zapis cez serial

//Do not add code below this line
#endif /* _M6050comp_H_ */
