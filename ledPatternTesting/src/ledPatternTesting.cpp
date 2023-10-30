/* 
 * Project Neopixel strip testing
 * Author: Your Name
 * Date: OCT-25-2023
 */

#include "Particle.h"
#include "IoTClassroom_CNM.h"
#include "Colors.h"
#include "neopixel.h"

//Neopixels
int pixelCount=80;
int staticColor;
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

void pixelFill(int startPixel, int endPixel, int fillColor);
void RGBpixelFill(int startPixel, int endPixel, float r, float g, float b);
void RGBfadePulse(int min, int max, byte red, byte green, byte blue);
void rainbowCycle(int wait);
int Wheel(byte WheelPos);
void RGBfadeWipe(int startPixel, int endPixel,int min, int max, int red, int green, int blue, int delaytime);
void spiralRun(int stripAmount, int pixelAmount,int fillColor, int delayTime);
void inOutBurst(int stripAmount, int pixelAmount,int fillColor, int delayTime);

void comet(int red, int green, int blue, int size, int decay, int SpeedDelay, int startPixel, int endPixel);
void fadeToBlack(int ledNo, byte fadeValue);



// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(SEMI_AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

int pixelCounter;
int stripCounter;

void setup() {
    Serial.begin(9600);
   // waitFor(Serial.isConnected, 5000);
    Serial.printf("Serial On \n");
    strip.begin();
    strip.setBrightness(255);
    strip.show();
    spiralRun(9,9,red,10);
    spiralRun(8,9,green,10);
    spiralRun(8,9,blue,10);
}

void loop() {

    inOutBurst(8,9,orange,2);
    inOutBurst(8,9,warmWhite,2);





/*  Burst Wipe   
    for(pixelCounter=9;pixelCounter>-1;pixelCounter--){
        pixelFill(pixelArray[0][pixelCounter], pixelArray[0][pixelCounter], warmWhite);
        pixelFill(pixelArray[1][pixelCounter], pixelArray[1][pixelCounter], white);
        pixelFill(pixelArray[2][pixelCounter], pixelArray[2][pixelCounter], warmWhite);
        pixelFill(pixelArray[3][pixelCounter], pixelArray[3][pixelCounter], white);
        pixelFill(pixelArray[4][pixelCounter], pixelArray[4][pixelCounter], warmWhite);
        pixelFill(pixelArray[5][pixelCounter], pixelArray[5][pixelCounter], white);
        pixelFill(pixelArray[6][pixelCounter], pixelArray[6][pixelCounter], warmWhite);
        pixelFill(pixelArray[7][pixelCounter], pixelArray[7][pixelCounter], white);
        delay(50);
      } 
      
      for(pixelCounter=9;pixelCounter>-1;pixelCounter--){
        pixelFill(pixelArray[0][pixelCounter], pixelArray[0][pixelCounter], black);
        pixelFill(pixelArray[1][pixelCounter], pixelArray[1][pixelCounter], black);
        pixelFill(pixelArray[2][pixelCounter], pixelArray[2][pixelCounter], black);
        pixelFill(pixelArray[3][pixelCounter], pixelArray[3][pixelCounter], black);
        pixelFill(pixelArray[4][pixelCounter], pixelArray[4][pixelCounter], black);
        pixelFill(pixelArray[5][pixelCounter], pixelArray[5][pixelCounter], black);
        pixelFill(pixelArray[6][pixelCounter], pixelArray[6][pixelCounter], black);
        pixelFill(pixelArray[7][pixelCounter], pixelArray[7][pixelCounter], black);
        delay(10);
      }  */
}

void pixelFill(int startPixel, int endPixel, int fillColor){
 	int i;
  for(i=startPixel; i<=endPixel; i++){
    strip.setPixelColor(i, fillColor);
  } 
  strip.show();
}

void spiralRun(int stripAmount, int pixelAmount,int fillColor, int delayTime){

  for(pixelCounter=9;pixelCounter>=0;pixelCounter--){
    for(stripCounter=0;stripCounter<8;stripCounter++){
        pixelFill(pixelArray[stripCounter][pixelCounter], pixelArray[stripCounter][pixelCounter], fillColor);
        delay(delayTime);
        }
  }
}

void inOutBurst(int stripAmount, int pixelAmount,int fillColor, int delayTime){
  
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

void RGBfadePulse(int min, int max, byte red, byte green, byte blue){
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

void comet(int red, int green, int blue, int size, int decay, int SpeedDelay, int startPixel, int endPixel) {  
    strip.clear();
    int i;
    int j;
    int pixelSpace = endPixel-startPixel;

  for(i = startPixel; i < endPixel; i++) {
    for(int j=startPixel; j<endPixel; j++) {
        fadeToBlack(j, decay );        

    } 
   
    for(int j =startPixel; j <endPixel ; j++) {
      if( ( i-j <pixelSpace) && (i-j>=startPixel) ) {
        strip.setPixelColor(i-j, red, green, blue);
      }
    }
   
    strip.show();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int pixel, byte fadeValue) {
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
   
    oldColor = strip.getPixelColor(pixel);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    strip.setPixelColor(pixel, r,g,b);
}

void rainbowCycle(int wait) {
  int i, j;
  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

int Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}