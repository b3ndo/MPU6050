// Do not remove the include below
#include "M6050comp.h"

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
//Adafruit_PCD8544 LCD = Adafruit_PCD8544(7, 6, 5, 4, 3);
// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 6 - Data/Command select (D/C)
// pin 5 - LCD chip select (CS)
// pin 4 - LCD reset (RST)
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!
Adafruit_PCD8544 LCD = Adafruit_PCD8544(6, 5, 4);
Buzzer hlasic(13, 2400, 500, 500);
HMC5883L compass;
MPU6050 mpu;
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void checkbatt()//kontrola bateriek
{
	analogReference(DEFAULT);//set internal aRef
//	battA = analogRead(battAin);
//	delay(10);
	battA = analogRead(battAPin)*0.004538;
//	delayMicroseconds(100);
//	battB = analogRead(battBin);
	delay(5);
	battB = analogRead(battBPin)*0.004538;
	if(battA < 3.1 || battB < 3.1) hlasic.Beep(200, 200);
	else if(battA < 3.3 || battB < 3.3) hlasic.Beep();
//	else hlasic.Off();
	analogReference(INTERNAL);//set internal aRef
}
void LCDbatt()
{
	LCD.setTextSize(1);
	LCD.setTextColor(BLACK);
	LCD.setCursor(0,0);
	LCD.print(battA);
	LCD.println(" V");
	LCD.print(battB);
	LCD.println(" V");
	LCD.display();
	LCD.clearDisplay();
}
void zobraz()
{
	LCD.setTextSize(1);
	LCD.setTextColor(BLACK);
	for(int i = 0; i<=5; i++)
	{
		LCD.setCursor(0,8*i);
		LCD.print(ponuka[i]);
		if(i==0 || i==1 || i==3 || i==4) LCD.print(data[i],1);
		if(i==2) LCD.print(data[i],2);
		if(i==5) LCD.print(data[i],0);
		if(i==2 || i==3) LCD.print("hPa");
	}
	LCD.display();
	LCD.clearDisplay();
}
void zapis()
{
	for(int i = 0; i<=6; i++)
	{
		if(i==0 || i==1 || i==3 || i==4 || i==6) Serial.print(data[i],1);
		if(i==2) Serial.print(data[i],2);
		if(i==5) Serial.print(data[i],0);
		Serial.print("\t");
	}
	Serial.print(battA, 2);
	Serial.print("V \t");
	Serial.print(battB, 2);
	Serial.println("V \t");
}
//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
	analogReference(INTERNAL);
//	analogReference(DEFAULT);
	pinMode(buttonPin,INPUT_PULLUP);
	pinMode(battAPin,INPUT);
	pinMode(battBPin,INPUT);
	LCD.begin(58);
	delay(500);
	LCD.clearDisplay();
	LCD.setRotation(0);
	LCD.setTextSize(1);
	LCD.setTextColor(BLACK);
	LCD.setCursor(20,0);
	LCD.println("MPU-6050");
	LCD.setCursor(40,15);
	LCD.println("+");
	LCD.setCursor(22,30);
	LCD.println("BMP-085");
	LCD.display();
	delay(1500);
	LCD.clearDisplay();
	Serial.begin(57600);
// If you have GY-86 or GY-87 module.
// To access HMC5883L you need to disable the I2C Master Mode and Sleep Mode, and enable I2C Bypass Mode
	while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
	{
		Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
	    delay(500);
	}
	while(!bmp.begin())
	{
	    Serial.println("Could not find a valid BMPU085 sensor, check wiring!");
	    delay(500);
	 }
	mpu.setI2CMasterModeEnabled(false);
	mpu.setI2CBypassEnabled(true) ;
	mpu.setSleepEnabled(false);
	// Initialize Initialize HMC5883L
	Serial.println("Initialize HMC5883L");
	while (!compass.begin())
	{
	    Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
	    delay(500);
	}
// Set measurement range
	compass.setRange(HMC5883L_RANGE_1_3GA);
// Set measurement mode
	compass.setMeasurementMode(HMC5883L_CONTINOUS);
// Set data rate
	compass.setDataRate(HMC5883L_DATARATE_15HZ);
// Set number of samples averaged
	compass.setSamples(HMC5883L_SAMPLES_8);
// Set calibration offset. See HMC5883L_calibration.ino
	compass.setOffset(-111, 56, -66); //0,0 povodne
}
// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
	float currentMillis = millis();
	if(!digitalRead(buttonPin))
	{
		if(ik > 0) seaLevelPressure=_pressure/ik/100;
		//TODO dorobit sealevel....pri stlaceni tlac
		LCDbatt();
	}
	if(currentMillis - prevMillisMeasur > 240)
	{
		ik++;
		sensors_event_t event;
		bmp.getEvent(&event);
		Vector norm = compass.readNormalize();
		heading = atan2(norm.YAxis, -norm.ZAxis);// Calculate heading
		heading += declinationAngle;
		if (heading < 0) heading += 2 * PI;// Correct for heading < 0deg and heading > 360deg
		if (heading > 2 * PI) heading -= 2 * PI;
		headingDegrees += heading * 180/M_PI;// Convert to degrees
		tempMPU += mpu.readTemperature();// Read temp from MPU
		bmp.getTemperature(&tempBMP);// Read temp from BMP
		_tempBMP += tempBMP;
		bmp.getPressure(&pressure);
		_pressure += pressure;
// Then convert the atmospheric pressure, and SLP to altitude
// Update this next line with the current SLP for better results
// Read altitude from BMP
		calculseaLevelPressure += bmp.seaLevelForAltitude(326, event.pressure);
		altitude += bmp.pressureToAltitude(seaLevelPressure, event.pressure);
		analogRead(LM35Pin);
		delay(5);
		LM35 += analogRead(LM35Pin)*0.242;
		prevMillisMeasur = currentMillis;
	}
	if(currentMillis - prevMillisData > 980 && ik == 4)
	{
		iz++;
		data[0] = _tempBMP/4; data[1] = tempMPU / 4;
		data[2] = _pressure/400; data[3] = calculseaLevelPressure / 4;
		data[4] = altitude / 4; data[5] = (int)headingDegrees / 4;
		data[6] = LM35 /4;
		zobraz();
		if(Serial){
			if (interval==iz)
			{
				zapis();
				iz = 0;
			}
		}
		else iz=0;
		tempBMP =  0; _tempBMP = 0; tempMPU = 0;
		pressure = 0; _pressure = 0; altitude = 0;
		headingDegrees = 0;	calculseaLevelPressure = 0;		//Prepoc. tlak na urovni mora
		ik = 0; LM35 = 0;
		checkbatt();
		prevMillisData = currentMillis;
	}
}


