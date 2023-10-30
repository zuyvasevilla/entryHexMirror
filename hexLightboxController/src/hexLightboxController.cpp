/* 
 * Project Hex Lightbox Smartroom Controller Midterm
 * Author: Zuyva Sevilla
 * Date: OCT-23-2023
 */

#include "Particle.h"
#include <SPI.h>
#include "IoTClassroom_CNM.h"
#include "MFRC522.h"
#include "MQ135.h"
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
#include "neopixel.h"
#include "colors.h"
#include "wemo.h"

//RFID
int RST_PIN = 12;         
int SS_PIN = 18;
byte readCard[4];
String keytagUID = "5E0453C3";
String ringUID = "0C443B3D";
String cardUID = "29E2F65E";
String scannedID = "";        //Where to write scanned ID tag 
MFRC522 READER(SS_PIN, RST_PIN);  // Create MFRC522 instance
bool RFIDflip;
bool checkID();
void RFIDcheck();

//Gas
MQ135 gasSensor = MQ135(11);
float rzero;
int ppm;
int co2PPM;

//Temp
Adafruit_BME280 bme;
int BMEstatus;
int BMEhex = 0x76;
int tempC;
float tempF;
int pressPA;
float pressinHG;
int humidRH;
char pCent = 0x25;
char degree = 0xF8;

//Buttons
const int greenButtonLED = 13;
Button greenButton(10);
Button redButton(7);
int activeNeoPattern;

//OLED
int OLED_RESET=D4;
Adafruit_SSD1306 display(OLED_RESET);

//Hue Lights
const int hueCount=7;
void hueRainbowFill (int startBulb, int endBulb);
void hueFill (int startBulb, int endBulb, int fillColor, int brightness, int sat);
void hueFillOff (int startBulb, int endBulb);

//Wemo
const int fanWemo=5;
const int filterWemo=0;
bool filterONoff;
bool airWarning;

//NeoPixels
int pixelCount=80;
int pixelArray[8][10]={
  {0,1,2,3,4,5,6,7,8,9},
  {19,18,17,16,15,14,13,12,11,10},
  {20,21,22,23,24,25,26,27,28,29},
  {39,38,37,36,35,34,33,32,31,30},
  {40,41,42,43,44,45,46,47,48,49},
  {59,58,57,56,55,54,53,52,51,50},
  {60,61,62,63,64,65,66,67,68,69},
  {79,78,77,76,75,74,73,72,71,70}
};
Adafruit_NeoPixel strip(pixelCount, SPI1, WS2812B);
//NeoPixel Functions
void pixelFill(int startPixel, int endPixel, int fillColor);
void outBurst(int stripAmount, int pixelAmount,int fillColor, int delayTime);
void spiralRun(int stripAmount, int pixelAmount,int fillColor, int delayTime);
void RGBpixelFill(int startPixel, int endPixel, float r, float g, float b);
void RGBpulse(int min, int max, int red, int green, int blue);
int neoRainbowIndex;

//Other Functions
void analogPulse(int LEDPIN, int min, int max, int delayTime);

SYSTEM_MODE(SEMI_AUTOMATIC);
SYSTEM_THREAD(ENABLED);

void setup() {
	Serial.begin(9600);		
	waitFor(Serial.isConnected, 5000);
  	Serial.printf("SERIAL READY\n");
/* 	WiFi.on();
  	WiFi.clearCredentials();
  	WiFi.setCredentials("IoTNetwork");
  	WiFi.connect();
  	while(WiFi.connecting()){
    	Serial.printf("."); 
  	}
  	delay(1000);
  	Serial.printf("\n\n"); */
	
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
 	display.display();
	display.setRotation(2);
 	delay(500);
  	display.clearDisplay();

	SPI.begin();			// Init SPI bus
	READER.PCD_Init();		// Init RFID MFRC522
	READER.PCD_DumpVersionToSerial();	// Show RFID Reader details
	Serial.printf("RFID READY\n");
	BMEstatus = bme.begin(BMEhex);
 	if (BMEstatus==false){
    	Serial.printf("BME280 at address 0x%02X failed to start", BMEhex);
 		}
	pinMode(greenButtonLED,OUTPUT);
	
	strip.begin();  //Init Neopixels
	strip.setBrightness(255);
	strip.show();
	spiralRun(9,9,red,10);
    spiralRun(8,9,green,10);
    spiralRun(8,9,blue,10);
    spiralRun(8,9,white,10);
	Serial.printf("NEOPIXELS READY \n");
	
	RFIDflip = FALSE;
	filterONoff = FALSE;
	airWarning = FALSE;
	activeNeoPattern = 0;
}

void loop() {
	//Sensors
	rzero = gasSensor.getRZero();  //Calibrates sensor? 
	co2PPM = gasSensor.getPPM();
    ppm = co2PPM / 4;

	tempC = bme.readTemperature();
  	tempF = ((tempC*1.8)+32);

  	pressPA = bme.readPressure();
  	pressinHG=pressPA/3386;
  	humidRH = bme.readHumidity();
    
	Serial.printf("CO2 PPM %i\n PPM %i\n Neo: %i", co2PPM, ppm, activeNeoPattern);
	
	if (ppm>40000){
		airWarning = HIGH;
	}
	else{
		airWarning = LOW;
	}
	
	if(greenButton.isClicked()) {   
    	filterONoff = !filterONoff;
  	}
	
	if (redButton.isClicked()) {  
      activeNeoPattern++;
      if (activeNeoPattern > 3) {  
	      activeNeoPattern = 0;
      	}
	}
	if (filterONoff == HIGH){
		wemoWrite(filterWemo, HIGH);
		digitalWrite(greenButtonLED, HIGH);
	}
	else{
		wemoWrite(filterWemo, LOW);
		digitalWrite(greenButtonLED, LOW);
	}
	while (checkID()){   //RFID Scanning
		if (scannedID == cardUID){
  		RFIDflip =! RFIDflip;
		READER.PICC_DumpToSerial(&(READER.uid));
		}
		else{
		READER.PICC_DumpToSerial(&(READER.uid));
		}
	}

/* 	if (READER.PICC_IsNewCardPresent() && READER.PICC_ReadCardSerial()) {
    	RFIDcheck();
		if (scannedID == cardUID){
  		RFIDflip =! RFIDflip;
		READER.PICC_DumpToSerial(&(READER.uid));
		}
		else{
		READER.PICC_DumpToSerial(&(READER.uid));
		}
  	 }*/
	 
	if(RFIDflip == HIGH){
		if(tempF > 65){
			wemoWrite(fanWemo, HIGH);
		}
		strip.setBrightness(255);
		display.clearDisplay();
 		display.setTextSize(1);
  		display.setTextColor(WHITE);
  		display.setCursor(0,0);
  		display.printf("ALARM OFF");
		display.setTextSize(2);
  		display.setCursor(0,20);
		display.printf("%.00f%cF\n%i PPM",tempF,degree,ppm);
  		display.display();

		switch (activeNeoPattern) {
      case 0:
		hueFill(1,hueCount,HueLightYellow,200, 15);
		spiralRun(8,4,orange,4);
    	spiralRun(8,9,white,7);
		break;
      case 1:
	  	hueRainbowFill(1,hueCount);
		outBurst(8,9, rainbow[neoRainbowIndex], 25);
		neoRainbowIndex++;
		if(neoRainbowIndex>7){
			neoRainbowIndex=0;
		}
        break;
      case 2:
	  	hueFill(1,hueCount,HueLightYellow,200, 15);
		pixelFill(0,pixelCount, white);
        break;
		}
	}
	else if (RFIDflip == HIGH && airWarning == HIGH){
		strip.clear();
		if(tempF > 65){
			wemoWrite(fanWemo, HIGH);
		}
		strip.setBrightness(255);
		spiralRun(8,9,green,0);
		display.clearDisplay();
 		display.setTextSize(2);
  		display.setTextColor(WHITE);
  		display.setCursor(0,15);
  		display.printf("AIR ALERT\nFILTER ON");
  		display.display();
	}
	else{
	 	display.clearDisplay();
 		display.setTextSize(3);
  		display.setTextColor(WHITE);
  		display.setCursor(0,0);
  		display.printf("ALARM\nON");
		display.setTextSize(2);
  		display.setCursor(65,37);
		display.printf("%.00f%cF",tempF,degree);
  		display.display(); 

 		setHue(1,true,HueRed,50,255);
 		hueFillOff (2, hueCount);
		wemoWrite(fanWemo, LOW);
		strip.setBrightness(50);
		RGBpulse(20,255, 255,0,0); 
		}

}

bool checkID(){     //RFID Key Checker
	int i;
	if( ! READER.PICC_IsNewCardPresent()){
		return FALSE;
	}
	if( ! READER.PICC_ReadCardSerial()){
		return FALSE;
	}
	scannedID = "";
	for(i=0; i<4; i++){
		readCard[i] = READER.uid.uidByte[i];
    	scannedID.concat(String(READER.uid.uidByte[i], HEX)); // Convert the UID to a single String
    }
    scannedID.toUpperCase();
    READER.PICC_HaltA(); // Stop reading
    return true;
}
void RFIDcheck(){
	int i;
	scannedID = "";
	for(i=0; i<4; i++){
		readCard[i] = READER.uid.uidByte[i];
    	scannedID.concat(String(READER.uid.uidByte[i], HEX)); // Convert the UID to a single String
    }
    scannedID.toUpperCase();
    READER.PICC_HaltA(); // Stop reading
}
void analogPulse(int LEDPIN, int min, int max, int delayTime){ //pulse LED (with delay tho)
	int i;

    for (i=min; i<max; i++){
       analogWrite(LEDPIN, i);
	   delay(delayTime);
    }
	for (i=max; i>min; i--){
       analogWrite(LEDPIN, i);
	   delay(delayTime);
    }
}

//Hue Functions
void hueFill (int startBulb, int endBulb, int fillColor, int brightness, int sat){
	int i;
	for (i=startBulb; i<=endBulb;i++){
		setHue(i,true,fillColor,brightness,sat);
	}
}
void hueFillOff (int startBulb, int endBulb){
	int i;
	for (i=startBulb; i<=endBulb;i++){
		setHue(i,false,0,0,0);
	}
}
void hueRainbowFill(int startBulb, int endBulb){
	int i;
	for (i=startBulb; i<=endBulb;i++){
		setHue(i,true,HueRainbow[i%7],255,255);
	}
}

//Neopixel
void pixelFill(int startPixel, int endPixel, int fillColor){
 	int i;
	for(i=startPixel; i<=endPixel; i++){
    	strip.setPixelColor(i, fillColor);
  	} 
  	strip.show();
}
void spiralRun(int stripAmount, int pixelAmount,int fillColor, int delayTime){
	int pixelCounter, stripCounter;
  for(pixelCounter=9;pixelCounter>=0;pixelCounter--){
    for(stripCounter=0;stripCounter<8;stripCounter++){
        pixelFill(pixelArray[stripCounter][pixelCounter], pixelArray[stripCounter][pixelCounter], fillColor);
        delay(delayTime);
        }
  }
}
void outBurst(int stripAmount, int pixelAmount,int fillColor, int delayTime){
	int pixelCounter, stripCounter;
  for(pixelCounter=9;pixelCounter>=0;pixelCounter--){
    for(stripCounter=0;stripCounter<8;stripCounter++)
      pixelFill(pixelArray[stripCounter][pixelCounter], pixelArray[stripCounter][pixelCounter], fillColor);
      delay(delayTime);
    }  
}
void RGBpixelFill(int startPixel, int endPixel, float r, float g, float b){
 	int i;
  for(i=startPixel; i<=endPixel; i++){
    strip.setPixelColor(i, r, g, b);
  } 
  strip.show();
}
void RGBpulse(int min, int max, int red, int green, int blue){
    float r, g, b;
    int i;
    for (i=min; i<max; i++){
        r = (i/256.0)*red;
        g = (i/256.0)*green;
        b = (i/256.0)*blue;
        RGBpixelFill(0, pixelCount, r, g, b);
        strip.show();
    }

    for (i=max; i>min; i--){
        r = (i/256.0)*red;
        g = (i/256.0)*green;
        b = (i/256.0)*blue;
        RGBpixelFill(0, pixelCount, r, g, b);
        strip.show();
    }
}

