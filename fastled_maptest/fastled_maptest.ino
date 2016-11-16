#include <FastLED.h>
//#include "MatrixMapping.h"

#define DEBUG
#define LED_PIN  6

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 255

int PIR1Pin = 15; // LEFT
int PIR2Pin = 16; // Up
int PIR3Pin = 17; // Right
int PIR4Pin = 18; // Bottom
int PIR1State = 0;
int PIR2State = 0;
int PIR3State = 0;
int PIR4State = 0;

int xSize = 10;
int ySize = 10;
int maxLEDList = 8;
int LEDMap[10][10][8];

// Params for width and height
const uint8_t kMatrixWidth = 10;
const uint8_t kMatrixHeight = 10;

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;



#define NUM_LEDS 240
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* leds( leds_plus_safety_pixel + 1);

void initArray();
void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
void LightLeft(uint32_t ms);
void LightRight(uint32_t ms);
void LightUp(uint32_t ms);
void LightDown(uint32_t ms);
void TestColors(uint32_t ms);
void ClearAll();

void PrintPIRStates()
{
	Serial.print("PIR1: ");
	Serial.print(PIR1State);
	Serial.print(" PIR2: ");
	Serial.print(PIR2State);
	Serial.print(" PIR3: ");
	Serial.print(PIR3State);
	Serial.print(" PIR4: ");
	Serial.println(PIR4State);
}

void ReadPIRSensors()
{
	PIR1State = digitalRead(PIR1Pin);
	PIR2State = digitalRead(PIR2Pin);
	PIR3State = digitalRead(PIR3Pin);
	PIR4State = digitalRead(PIR4Pin);
}

void setupPIR() 
{
	pinMode(PIR1Pin, INPUT);
	pinMode(PIR2Pin, INPUT);
	pinMode(PIR3Pin, INPUT);
	pinMode(PIR4Pin, INPUT);
}

void XY( uint8_t x, uint8_t y, uint8_t nHue, boolean off = false)
{ 
      int ledToWrite;
      for(int i = 0; i < 8; i++){
        
        ledToWrite = LEDMap[x][y][i];
        
        if(off)
        {
        	if(ledToWrite >= 0) leds[ ledToWrite ]  = CHSV( 0,0,0);
        }
        else
        {
        	if(ledToWrite >= 0) leds[ ledToWrite ]  = CHSV( nHue, 255, 255);
        	else return;
        }
      
      }
      
  
}

// Demo that USES "XY" follows code below

void loop()
{
	ReadPIRSensors();

#ifdef DEBUG
	PrintPIRStates();
#endif

    uint32_t ms = millis();
    // int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
    // int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
    // DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    //TestColors(ms);

    // ClearAll();
    // if(PIR1State)
    // 	LightLeft(ms);
    // if(PIR2State)
    // 	LightUp(ms);
    // if(PIR3State)
    // 	LightRight(ms);
    // if(PIR4State)
    // 	LightDown(ms);

    LightLeft(ms);

    if( ms < 5000 ) {
      FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
}

void ClearAll()
{
	for(byte y = 0; y < kMatrixHeight; y++) {
  		for(byte x = 0; x < kMatrixWidth; x++){
  			XY(x,y, 0, true);
  	}
  }
}

void TestColors(uint32_t ms)
{
	ClearAll();

	if(ms < 5000){
		LightRight(ms);
	}
	else if(ms < 7000){
		LightUp(ms);
	}
	else if(ms < 9000){
		LightLeft(ms);
	}
	else {
		LightDown(ms);
	}
}

void LightRight(uint32_t ms)
{
	for(byte y = 0; y < kMatrixHeight; y++) {
  		for(byte x = 0; x < kMatrixWidth/2; x++){
  			XY(x,y, 0);
  	}
  }
}

void LightUp(uint32_t ms)
{
	int yRow = (ms/100)%kMatrixHeight;

	for(byte y = 0; y < kMatrixHeight; y++) {
		if(y==yRow){
  			for(byte x = 0; x < kMatrixWidth; x++){
  				XY(x,y, 64);
  			}
  		}
  		else{
  			for(byte x = 0; x < kMatrixWidth; x++){
  				XY(x,y,0);
  			}
  		}
  	}
}

void LightDown(uint32_t ms)
{
	for(byte y = kMatrixHeight/2; y < kMatrixHeight; y++) {
  		for(byte x = 0; x < kMatrixWidth; x++){
  			XY(x,y, 128);
  	}
  }
}

void LightLeft(uint32_t ms)
{
  int xRow = (ms/100)%kMatrixWidth;

  int interpolationDistance = 4;

  for(byte y = 0; y < kMatrixHeight; y++) {
  	for(byte x = 0; x < kMatrixWidth; x++){
  		if(x == xRow)
  			XY(x,y, 192);
  		else if(x < xRow - interpolationDistance)
  			XY(x,y, (192 -(192-150))/(abs(x-interpolationDistance)));
  		else if(x > xRow + interpolationDistance)
  			XY(x,y, (192 -(192-150))/(abs(x-interpolationDistance)));
  		else
  			XY(x,y, 150);
  	}
  }
}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  
  // Quadrant 1
  for(byte y = 0; y < kMatrixHeight/2; y++) {
  	for(byte x = 0; x < kMatrixWidth/2; x++){
  		XY(x,y, 0);
  	}
  }

 // Quadrant 2
 for(byte y = kMatrixHeight/2; y < kMatrixHeight; y++) {
  	for(byte x = 0; x < kMatrixWidth/2; x++){
  		XY(x,y, 64);
  	}
  }

  // Quadrant 3
 for(byte y = kMatrixHeight/2; y < kMatrixHeight; y++) {
  	for(byte x = kMatrixWidth/2; x < kMatrixWidth; x++){
  		XY(x,y, 128);
  	}
  }

 // Quadrant 4
 for(byte y = 0; y < kMatrixHeight/2; y++) {
  	for(byte x = kMatrixWidth/2; x < kMatrixWidth; x++){
  		XY(x,y, 192);
  	}
  }

}

void setup() {
  Serial.begin(9600);

  setupPIR();
  initArray();
  
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
}

void initArray(){
  LEDMap[0][0][0] = 52;
  LEDMap[0][0][1] = 53;
  LEDMap[0][0][2] = 54;
  LEDMap[0][0][3] = 55;
  LEDMap[0][0][4] = -1;
  LEDMap[0][0][5] = -1;
  LEDMap[0][0][6] = -1;
  LEDMap[0][0][7] = -1;
  LEDMap[1][0][0] = 56;
  LEDMap[1][0][1] = 57;
  LEDMap[1][0][2] = 58;
  LEDMap[1][0][3] = -1;
  LEDMap[1][0][4] = -1;
  LEDMap[1][0][5] = -1;
  LEDMap[1][0][6] = -1;
  LEDMap[1][0][7] = -1;
  LEDMap[2][0][0] = 60;
  LEDMap[2][0][1] = 61;
  LEDMap[2][0][2] = -1;
  LEDMap[2][0][3] = -1;
  LEDMap[2][0][4] = -1;
  LEDMap[2][0][5] = -1;
  LEDMap[2][0][6] = -1;
  LEDMap[2][0][7] = -1;
  LEDMap[3][0][0] = 62;
  LEDMap[3][0][1] = 63;
  LEDMap[3][0][2] = 64;
  LEDMap[3][0][3] = -1;
  LEDMap[3][0][4] = -1;
  LEDMap[3][0][5] = -1;
  LEDMap[3][0][6] = -1;
  LEDMap[3][0][7] = -1;
  LEDMap[4][0][0] = 65;
  LEDMap[4][0][1] = 66;
  LEDMap[4][0][2] = 67;
  LEDMap[4][0][3] = -1;
  LEDMap[4][0][4] = -1;
  LEDMap[4][0][5] = -1;
  LEDMap[4][0][6] = -1;
  LEDMap[4][0][7] = -1;
  LEDMap[5][0][0] = 68;
  LEDMap[5][0][1] = 69;
  LEDMap[5][0][2] = 70;
  LEDMap[5][0][3] = -1;
  LEDMap[5][0][4] = -1;
  LEDMap[5][0][5] = -1;
  LEDMap[5][0][6] = -1;
  LEDMap[5][0][7] = -1;
  LEDMap[6][0][0] = 71;
  LEDMap[6][0][1] = 72;
  LEDMap[6][0][2] = 73;
  LEDMap[6][0][3] = -1;
  LEDMap[6][0][4] = -1;
  LEDMap[6][0][5] = -1;
  LEDMap[6][0][6] = -1;
  LEDMap[6][0][7] = -1;
  LEDMap[7][0][0] = 74;
  LEDMap[7][0][1] = 75;
  LEDMap[7][0][2] = 76;
  LEDMap[7][0][3] = -1;
  LEDMap[7][0][4] = -1;
  LEDMap[7][0][5] = -1;
  LEDMap[7][0][6] = -1;
  LEDMap[7][0][7] = -1;
  LEDMap[8][0][0] = 77;
  LEDMap[8][0][1] = 78;
  LEDMap[8][0][2] = 79;
  LEDMap[8][0][3] = -1;
  LEDMap[8][0][4] = -1;
  LEDMap[8][0][5] = -1;
  LEDMap[8][0][6] = -1;
  LEDMap[8][0][7] = -1;
  LEDMap[9][0][0] = 80;
  LEDMap[9][0][1] = 81;
  LEDMap[9][0][2] = -1;
  LEDMap[9][0][3] = -1;
  LEDMap[9][0][4] = -1;
  LEDMap[9][0][5] = -1;
  LEDMap[9][0][6] = -1;
  LEDMap[9][0][7] = -1;
  LEDMap[0][1][0] = 50;
  LEDMap[0][1][1] = 51;
  LEDMap[0][1][2] = -1;
  LEDMap[0][1][3] = -1;
  LEDMap[0][1][4] = -1;
  LEDMap[0][1][5] = -1;
  LEDMap[0][1][6] = -1;
  LEDMap[0][1][7] = -1;
  LEDMap[1][1][0] = 135;
  LEDMap[1][1][1] = 136;
  LEDMap[1][1][2] = 137;
  LEDMap[1][1][3] = 138;
  LEDMap[1][1][4] = -1;
  LEDMap[1][1][5] = -1;
  LEDMap[1][1][6] = -1;
  LEDMap[1][1][7] = -1;
  LEDMap[2][1][0] = 139;
  LEDMap[2][1][1] = 140;
  LEDMap[2][1][2] = -1;
  LEDMap[2][1][3] = -1;
  LEDMap[2][1][4] = -1;
  LEDMap[2][1][5] = -1;
  LEDMap[2][1][6] = -1;
  LEDMap[2][1][7] = -1;
  LEDMap[3][1][0] = 141;
  LEDMap[3][1][1] = 142;
  LEDMap[3][1][2] = 143;
  LEDMap[3][1][3] = -1;
  LEDMap[3][1][4] = -1;
  LEDMap[3][1][5] = -1;
  LEDMap[3][1][6] = -1;
  LEDMap[3][1][7] = -1;
  LEDMap[4][1][0] = 144;
  LEDMap[4][1][1] = 145;
  LEDMap[4][1][2] = 146;
  LEDMap[4][1][3] = -1;
  LEDMap[4][1][4] = -1;
  LEDMap[4][1][5] = -1;
  LEDMap[4][1][6] = -1;
  LEDMap[4][1][7] = -1;
  LEDMap[5][1][0] = 147;
  LEDMap[5][1][1] = 148;
  LEDMap[5][1][2] = 149;
  LEDMap[5][1][3] = -1;
  LEDMap[5][1][4] = -1;
  LEDMap[5][1][5] = -1;
  LEDMap[5][1][6] = -1;
  LEDMap[5][1][7] = -1;
  LEDMap[6][1][0] = 150;
  LEDMap[6][1][1] = 151;
  LEDMap[6][1][2] = 152;
  LEDMap[6][1][3] = -1;
  LEDMap[6][1][4] = -1;
  LEDMap[6][1][5] = -1;
  LEDMap[6][1][6] = -1;
  LEDMap[6][1][7] = -1;
  LEDMap[7][1][0] = 153;
  LEDMap[7][1][1] = 154;
  LEDMap[7][1][2] = 155;
  LEDMap[7][1][3] = 156;
  LEDMap[7][1][4] = -1;
  LEDMap[7][1][5] = -1;
  LEDMap[7][1][6] = -1;
  LEDMap[7][1][7] = -1;
  LEDMap[8][1][0] = 157;
  LEDMap[8][1][1] = 158;
  LEDMap[8][1][2] = -1;
  LEDMap[8][1][3] = -1;
  LEDMap[8][1][4] = -1;
  LEDMap[8][1][5] = -1;
  LEDMap[8][1][6] = -1;
  LEDMap[8][1][7] = -1;
  LEDMap[9][1][0] = 82;
  LEDMap[9][1][1] = 83;
  LEDMap[9][1][2] = 84;
  LEDMap[9][1][3] = -1;
  LEDMap[9][1][4] = -1;
  LEDMap[9][1][5] = -1;
  LEDMap[9][1][6] = -1;
  LEDMap[9][1][7] = -1;
  LEDMap[0][2][0] = 47;
  LEDMap[0][2][1] = 48;
  LEDMap[0][2][2] = 49;
  LEDMap[0][2][3] = -1;
  LEDMap[0][2][4] = -1;
  LEDMap[0][2][5] = -1;
  LEDMap[0][2][6] = -1;
  LEDMap[0][2][7] = -1;
  LEDMap[1][2][0] = 133;
  LEDMap[1][2][1] = 134;
  LEDMap[1][2][2] = 201;
  LEDMap[1][2][3] = -1;
  LEDMap[1][2][4] = -1;
  LEDMap[1][2][5] = -1;
  LEDMap[1][2][6] = -1;
  LEDMap[1][2][7] = -1;
  LEDMap[2][2][0] = 202;
  LEDMap[2][2][1] = 203;
  LEDMap[2][2][2] = 204;
  LEDMap[2][2][3] = -1;
  LEDMap[2][2][4] = -1;
  LEDMap[2][2][5] = -1;
  LEDMap[2][2][6] = -1;
  LEDMap[2][2][7] = -1;
  LEDMap[3][2][0] = 205;
  LEDMap[3][2][1] = 206;
  LEDMap[3][2][2] = 207;
  LEDMap[3][2][3] = -1;
  LEDMap[3][2][4] = -1;
  LEDMap[3][2][5] = -1;
  LEDMap[3][2][6] = -1;
  LEDMap[3][2][7] = -1;
  LEDMap[4][2][0] = 208;
  LEDMap[4][2][1] = 209;
  LEDMap[4][2][2] = 210;
  LEDMap[4][2][3] = -1;
  LEDMap[4][2][4] = -1;
  LEDMap[4][2][5] = -1;
  LEDMap[4][2][6] = -1;
  LEDMap[4][2][7] = -1;
  LEDMap[5][2][0] = 211;
  LEDMap[5][2][1] = 212;
  LEDMap[5][2][2] = 213;
  LEDMap[5][2][3] = -1;
  LEDMap[5][2][4] = -1;
  LEDMap[5][2][5] = -1;
  LEDMap[5][2][6] = -1;
  LEDMap[5][2][7] = -1;
  LEDMap[6][2][0] = 214;
  LEDMap[6][2][1] = 215;
  LEDMap[6][2][2] = 216;
  LEDMap[6][2][3] = -1;
  LEDMap[6][2][4] = -1;
  LEDMap[6][2][5] = -1;
  LEDMap[6][2][6] = -1;
  LEDMap[6][2][7] = -1;
  LEDMap[7][2][0] = 217;
  LEDMap[7][2][1] = 218;
  LEDMap[7][2][2] = 219;
  LEDMap[7][2][3] = 220;
  LEDMap[7][2][4] = -1;
  LEDMap[7][2][5] = -1;
  LEDMap[7][2][6] = -1;
  LEDMap[7][2][7] = -1;
  LEDMap[8][2][0] = 159;
  LEDMap[8][2][1] = 160;
  LEDMap[8][2][2] = 161;
  LEDMap[8][2][3] = -1;
  LEDMap[8][2][4] = -1;
  LEDMap[8][2][5] = -1;
  LEDMap[8][2][6] = -1;
  LEDMap[8][2][7] = -1;
  LEDMap[9][2][0] = 85;
  LEDMap[9][2][1] = 86;
  LEDMap[9][2][2] = -1;
  LEDMap[9][2][3] = -1;
  LEDMap[9][2][4] = -1;
  LEDMap[9][2][5] = -1;
  LEDMap[9][2][6] = -1;
  LEDMap[9][2][7] = -1;
  LEDMap[0][3][0] = 45;
  LEDMap[0][3][1] = -1;
  LEDMap[0][3][2] = -1;
  LEDMap[0][3][3] = -1;
  LEDMap[0][3][4] = -1;
  LEDMap[0][3][5] = -1;
  LEDMap[0][3][6] = -1;
  LEDMap[0][3][7] = -1;
  LEDMap[1][3][0] = 131;
  LEDMap[1][3][1] = 132;
  LEDMap[1][3][2] = 199;
  LEDMap[1][3][3] = 200;
  LEDMap[1][3][4] = -1;
  LEDMap[1][3][5] = -1;
  LEDMap[1][3][6] = -1;
  LEDMap[1][3][7] = -1;
  LEDMap[2][3][0] = -1;
  LEDMap[2][3][1] = -1;
  LEDMap[2][3][2] = -1;
  LEDMap[2][3][3] = -1;
  LEDMap[2][3][4] = -1;
  LEDMap[2][3][5] = -1;
  LEDMap[2][3][6] = -1;
  LEDMap[2][3][7] = -1;
  LEDMap[3][3][0] = -1;
  LEDMap[3][3][1] = -1;
  LEDMap[3][3][2] = -1;
  LEDMap[3][3][3] = -1;
  LEDMap[3][3][4] = -1;
  LEDMap[3][3][5] = -1;
  LEDMap[3][3][6] = -1;
  LEDMap[3][3][7] = -1;
  LEDMap[4][3][0] = -1;
  LEDMap[4][3][1] = -1;
  LEDMap[4][3][2] = -1;
  LEDMap[4][3][3] = -1;
  LEDMap[4][3][4] = -1;
  LEDMap[4][3][5] = -1;
  LEDMap[4][3][6] = -1;
  LEDMap[4][3][7] = -1;
  LEDMap[5][3][0] = -1;
  LEDMap[5][3][1] = -1;
  LEDMap[5][3][2] = -1;
  LEDMap[5][3][3] = -1;
  LEDMap[5][3][4] = -1;
  LEDMap[5][3][5] = -1;
  LEDMap[5][3][6] = -1;
  LEDMap[5][3][7] = -1;
  LEDMap[6][3][0] = -1;
  LEDMap[6][3][1] = -1;
  LEDMap[6][3][2] = -1;
  LEDMap[6][3][3] = -1;
  LEDMap[6][3][4] = -1;
  LEDMap[6][3][5] = -1;
  LEDMap[6][3][6] = -1;
  LEDMap[6][3][7] = -1;
  LEDMap[7][3][0] = 221;
  LEDMap[7][3][1] = -1;
  LEDMap[7][3][2] = -1;
  LEDMap[7][3][3] = -1;
  LEDMap[7][3][4] = -1;
  LEDMap[7][3][5] = -1;
  LEDMap[7][3][6] = -1;
  LEDMap[7][3][7] = -1;
  LEDMap[8][3][0] = 162;
  LEDMap[8][3][1] = 163;
  LEDMap[8][3][2] = 222;
  LEDMap[8][3][3] = 223;
  LEDMap[8][3][4] = -1;
  LEDMap[8][3][5] = -1;
  LEDMap[8][3][6] = -1;
  LEDMap[8][3][7] = -1;
  LEDMap[9][3][0] = 87;
  LEDMap[9][3][1] = 88;
  LEDMap[9][3][2] = 89;
  LEDMap[9][3][3] = -1;
  LEDMap[9][3][4] = -1;
  LEDMap[9][3][5] = -1;
  LEDMap[9][3][6] = -1;
  LEDMap[9][3][7] = -1;
  LEDMap[0][4][0] = 43;
  LEDMap[0][4][1] = 44;
  LEDMap[0][4][2] = 129;
  LEDMap[0][4][3] = -1;
  LEDMap[0][4][4] = -1;
  LEDMap[0][4][5] = -1;
  LEDMap[0][4][6] = -1;
  LEDMap[0][4][7] = -1;
  LEDMap[1][4][0] = 130;
  LEDMap[1][4][1] = 196;
  LEDMap[1][4][2] = 197;
  LEDMap[1][4][3] = 198;
  LEDMap[1][4][4] = -1;
  LEDMap[1][4][5] = -1;
  LEDMap[1][4][6] = -1;
  LEDMap[1][4][7] = -1;
  LEDMap[2][4][0] = -1;
  LEDMap[2][4][1] = -1;
  LEDMap[2][4][2] = -1;
  LEDMap[2][4][3] = -1;
  LEDMap[2][4][4] = -1;
  LEDMap[2][4][5] = -1;
  LEDMap[2][4][6] = -1;
  LEDMap[2][4][7] = -1;
  LEDMap[3][4][0] = -1;
  LEDMap[3][4][1] = -1;
  LEDMap[3][4][2] = -1;
  LEDMap[3][4][3] = -1;
  LEDMap[3][4][4] = -1;
  LEDMap[3][4][5] = -1;
  LEDMap[3][4][6] = -1;
  LEDMap[3][4][7] = -1;
  LEDMap[4][4][0] = -1;
  LEDMap[4][4][1] = -1;
  LEDMap[4][4][2] = -1;
  LEDMap[4][4][3] = -1;
  LEDMap[4][4][4] = -1;
  LEDMap[4][4][5] = -1;
  LEDMap[4][4][6] = -1;
  LEDMap[4][4][7] = -1;
  LEDMap[5][4][0] = -1;
  LEDMap[5][4][1] = -1;
  LEDMap[5][4][2] = -1;
  LEDMap[5][4][3] = -1;
  LEDMap[5][4][4] = -1;
  LEDMap[5][4][5] = -1;
  LEDMap[5][4][6] = -1;
  LEDMap[5][4][7] = -1;
  LEDMap[6][4][0] = -1;
  LEDMap[6][4][1] = -1;
  LEDMap[6][4][2] = -1;
  LEDMap[6][4][3] = -1;
  LEDMap[6][4][4] = -1;
  LEDMap[6][4][5] = -1;
  LEDMap[6][4][6] = -1;
  LEDMap[6][4][7] = -1;
  LEDMap[7][4][0] = -1;
  LEDMap[7][4][1] = -1;
  LEDMap[7][4][2] = -1;
  LEDMap[7][4][3] = -1;
  LEDMap[7][4][4] = -1;
  LEDMap[7][4][5] = -1;
  LEDMap[7][4][6] = -1;
  LEDMap[7][4][7] = -1;
  LEDMap[8][4][0] = 164;
  LEDMap[8][4][1] = 165;
  LEDMap[8][4][2] = 166;
  LEDMap[8][4][3] = 224;
  LEDMap[8][4][4] = 225;
  LEDMap[8][4][5] = -1;
  LEDMap[8][4][6] = -1;
  LEDMap[8][4][7] = -1;
  LEDMap[9][4][0] = 90;
  LEDMap[9][4][1] = 91;
  LEDMap[9][4][2] = -1;
  LEDMap[9][4][3] = -1;
  LEDMap[9][4][4] = -1;
  LEDMap[9][4][5] = -1;
  LEDMap[9][4][6] = -1;
  LEDMap[9][4][7] = -1;
  LEDMap[0][5][0] = 40;
  LEDMap[0][5][1] = 41;
  LEDMap[0][5][2] = 42;
  LEDMap[0][5][3] = 126;
  LEDMap[0][5][4] = 128;
  LEDMap[0][5][5] = -1;
  LEDMap[0][5][6] = -1;
  LEDMap[0][5][7] = -1;
  LEDMap[1][5][0] = 127;
  LEDMap[1][5][1] = 194;
  LEDMap[1][5][2] = 195;
  LEDMap[1][5][3] = -1;
  LEDMap[1][5][4] = -1;
  LEDMap[1][5][5] = -1;
  LEDMap[1][5][6] = -1;
  LEDMap[1][5][7] = -1;
  LEDMap[2][5][0] = -1;
  LEDMap[2][5][1] = -1;
  LEDMap[2][5][2] = -1;
  LEDMap[2][5][3] = -1;
  LEDMap[2][5][4] = -1;
  LEDMap[2][5][5] = -1;
  LEDMap[2][5][6] = -1;
  LEDMap[2][5][7] = -1;
  LEDMap[3][5][0] = -1;
  LEDMap[3][5][1] = -1;
  LEDMap[3][5][2] = -1;
  LEDMap[3][5][3] = -1;
  LEDMap[3][5][4] = -1;
  LEDMap[3][5][5] = -1;
  LEDMap[3][5][6] = -1;
  LEDMap[3][5][7] = -1;
  LEDMap[4][5][0] = -1;
  LEDMap[4][5][1] = -1;
  LEDMap[4][5][2] = -1;
  LEDMap[4][5][3] = -1;
  LEDMap[4][5][4] = -1;
  LEDMap[4][5][5] = -1;
  LEDMap[4][5][6] = -1;
  LEDMap[4][5][7] = -1;
  LEDMap[5][5][0] = -1;
  LEDMap[5][5][1] = -1;
  LEDMap[5][5][2] = -1;
  LEDMap[5][5][3] = -1;
  LEDMap[5][5][4] = -1;
  LEDMap[5][5][5] = -1;
  LEDMap[5][5][6] = -1;
  LEDMap[5][5][7] = -1;
  LEDMap[6][5][0] = -1;
  LEDMap[6][5][1] = -1;
  LEDMap[6][5][2] = -1;
  LEDMap[6][5][3] = -1;
  LEDMap[6][5][4] = -1;
  LEDMap[6][5][5] = -1;
  LEDMap[6][5][6] = -1;
  LEDMap[6][5][7] = -1;
  LEDMap[7][5][0] = -1;
  LEDMap[7][5][1] = -1;
  LEDMap[7][5][2] = -1;
  LEDMap[7][5][3] = -1;
  LEDMap[7][5][4] = -1;
  LEDMap[7][5][5] = -1;
  LEDMap[7][5][6] = -1;
  LEDMap[7][5][7] = -1;
  LEDMap[8][5][0] = 167;
  LEDMap[8][5][1] = 168;
  LEDMap[8][5][2] = 226;
  LEDMap[8][5][3] = 227;
  LEDMap[8][5][4] = -1;
  LEDMap[8][5][5] = -1;
  LEDMap[8][5][6] = -1;
  LEDMap[8][5][7] = -1;
  LEDMap[9][5][0] = 92;
  LEDMap[9][5][1] = 93;
  LEDMap[9][5][2] = 94;
  LEDMap[9][5][3] = -1;
  LEDMap[9][5][4] = -1;
  LEDMap[9][5][5] = -1;
  LEDMap[9][5][6] = -1;
  LEDMap[9][5][7] = -1;
  LEDMap[0][6][0] = 38;
  LEDMap[0][6][1] = 39;
  LEDMap[0][6][2] = 124;
  LEDMap[0][6][3] = 125;
  LEDMap[0][6][4] = -1;
  LEDMap[0][6][5] = -1;
  LEDMap[0][6][6] = -1;
  LEDMap[0][6][7] = -1;
  LEDMap[1][6][0] = 191;
  LEDMap[1][6][1] = 192;
  LEDMap[1][6][2] = 193;
  LEDMap[1][6][3] = -1;
  LEDMap[1][6][4] = -1;
  LEDMap[1][6][5] = -1;
  LEDMap[1][6][6] = -1;
  LEDMap[1][6][7] = -1;
  LEDMap[2][6][0] = 188;
  LEDMap[2][6][1] = 189;
  LEDMap[2][6][2] = 190;
  LEDMap[2][6][3] = -1;
  LEDMap[2][6][4] = -1;
  LEDMap[2][6][5] = -1;
  LEDMap[2][6][6] = -1;
  LEDMap[2][6][7] = -1;
  LEDMap[3][6][0] = 185;
  LEDMap[3][6][1] = 186;
  LEDMap[3][6][2] = 187;
  LEDMap[3][6][3] = -1;
  LEDMap[3][6][4] = -1;
  LEDMap[3][6][5] = -1;
  LEDMap[3][6][6] = -1;
  LEDMap[3][6][7] = -1;
  LEDMap[4][6][0] = 182;
  LEDMap[4][6][1] = 183;
  LEDMap[4][6][2] = 184;
  LEDMap[4][6][3] = -1;
  LEDMap[4][6][4] = -1;
  LEDMap[4][6][5] = -1;
  LEDMap[4][6][6] = -1;
  LEDMap[4][6][7] = -1;
  LEDMap[5][6][0] = 179;
  LEDMap[5][6][1] = 180;
  LEDMap[5][6][2] = 181;
  LEDMap[5][6][3] = -1;
  LEDMap[5][6][4] = -1;
  LEDMap[5][6][5] = -1;
  LEDMap[5][6][6] = -1;
  LEDMap[5][6][7] = -1;
  LEDMap[6][6][0] = 176;
  LEDMap[6][6][1] = 177;
  LEDMap[6][6][2] = 178;
  LEDMap[6][6][3] = -1;
  LEDMap[6][6][4] = -1;
  LEDMap[6][6][5] = -1;
  LEDMap[6][6][6] = -1;
  LEDMap[6][6][7] = -1;
  LEDMap[7][6][0] = 173;
  LEDMap[7][6][1] = 174;
  LEDMap[7][6][2] = 175;
  LEDMap[7][6][3] = -1;
  LEDMap[7][6][4] = -1;
  LEDMap[7][6][5] = -1;
  LEDMap[7][6][6] = -1;
  LEDMap[7][6][7] = -1;
  LEDMap[8][6][0] = 96;
  LEDMap[8][6][1] = 169;
  LEDMap[8][6][2] = 170;
  LEDMap[8][6][3] = 171;
  LEDMap[8][6][4] = 172;
  LEDMap[8][6][5] = 228;
  LEDMap[8][6][6] = 229;
  LEDMap[8][6][7] = 230;
  LEDMap[9][6][0] = 95;
  LEDMap[9][6][1] = 231;
  LEDMap[9][6][2] = 232;
  LEDMap[9][6][3] = -1;
  LEDMap[9][6][4] = -1;
  LEDMap[9][6][5] = -1;
  LEDMap[9][6][6] = -1;
  LEDMap[9][6][7] = -1;
  LEDMap[0][7][0] = 36;
  LEDMap[0][7][1] = 37;
  LEDMap[0][7][2] = 122;
  LEDMap[0][7][3] = 123;
  LEDMap[0][7][4] = -1;
  LEDMap[0][7][5] = -1;
  LEDMap[0][7][6] = -1;
  LEDMap[0][7][7] = -1;
  LEDMap[1][7][0] = 119;
  LEDMap[1][7][1] = 120;
  LEDMap[1][7][2] = 121;
  LEDMap[1][7][3] = -1;
  LEDMap[1][7][4] = -1;
  LEDMap[1][7][5] = -1;
  LEDMap[1][7][6] = -1;
  LEDMap[1][7][7] = -1;
  LEDMap[2][7][0] = 116;
  LEDMap[2][7][1] = 117;
  LEDMap[2][7][2] = 118;
  LEDMap[2][7][3] = -1;
  LEDMap[2][7][4] = -1;
  LEDMap[2][7][5] = -1;
  LEDMap[2][7][6] = -1;
  LEDMap[2][7][7] = -1;
  LEDMap[3][7][0] = 113;
  LEDMap[3][7][1] = 114;
  LEDMap[3][7][2] = 115;
  LEDMap[3][7][3] = -1;
  LEDMap[3][7][4] = -1;
  LEDMap[3][7][5] = -1;
  LEDMap[3][7][6] = -1;
  LEDMap[3][7][7] = -1;
  LEDMap[4][7][0] = 111;
  LEDMap[4][7][1] = 112;
  LEDMap[4][7][2] = -1;
  LEDMap[4][7][3] = -1;
  LEDMap[4][7][4] = -1;
  LEDMap[4][7][5] = -1;
  LEDMap[4][7][6] = -1;
  LEDMap[4][7][7] = -1;
  LEDMap[5][7][0] = 107;
  LEDMap[5][7][1] = 108;
  LEDMap[5][7][2] = 109;
  LEDMap[5][7][3] = 110;
  LEDMap[5][7][4] = -1;
  LEDMap[5][7][5] = -1;
  LEDMap[5][7][6] = -1;
  LEDMap[5][7][7] = -1;
  LEDMap[6][7][0] = 104;
  LEDMap[6][7][1] = 105;
  LEDMap[6][7][2] = 106;
  LEDMap[6][7][3] = -1;
  LEDMap[6][7][4] = -1;
  LEDMap[6][7][5] = -1;
  LEDMap[6][7][6] = -1;
  LEDMap[6][7][7] = -1;
  LEDMap[7][7][0] = 101;
  LEDMap[7][7][1] = 102;
  LEDMap[7][7][2] = 103;
  LEDMap[7][7][3] = -1;
  LEDMap[7][7][4] = -1;
  LEDMap[7][7][5] = -1;
  LEDMap[7][7][6] = -1;
  LEDMap[7][7][7] = -1;
  LEDMap[8][7][0] = 97;
  LEDMap[8][7][1] = 98;
  LEDMap[8][7][2] = 99;
  LEDMap[8][7][3] = 100;
  LEDMap[8][7][4] = -1;
  LEDMap[8][7][5] = -1;
  LEDMap[8][7][6] = -1;
  LEDMap[8][7][7] = -1;
  LEDMap[9][7][0] = 1;
  LEDMap[9][7][1] = 2;
  LEDMap[9][7][2] = -1;
  LEDMap[9][7][3] = -1;
  LEDMap[9][7][4] = -1;
  LEDMap[9][7][5] = -1;
  LEDMap[9][7][6] = -1;
  LEDMap[9][7][7] = -1;
  LEDMap[0][8][0] = 33;
  LEDMap[0][8][1] = 34;
  LEDMap[0][8][2] = 35;
  LEDMap[0][8][3] = -1;
  LEDMap[0][8][4] = -1;
  LEDMap[0][8][5] = -1;
  LEDMap[0][8][6] = -1;
  LEDMap[0][8][7] = -1;
  LEDMap[1][8][0] = -1;
  LEDMap[1][8][1] = -1;
  LEDMap[1][8][2] = -1;
  LEDMap[1][8][3] = -1;
  LEDMap[1][8][4] = -1;
  LEDMap[1][8][5] = -1;
  LEDMap[1][8][6] = -1;
  LEDMap[1][8][7] = -1;
  LEDMap[2][8][0] = -1;
  LEDMap[2][8][1] = -1;
  LEDMap[2][8][2] = -1;
  LEDMap[2][8][3] = -1;
  LEDMap[2][8][4] = -1;
  LEDMap[2][8][5] = -1;
  LEDMap[2][8][6] = -1;
  LEDMap[2][8][7] = -1;
  LEDMap[3][8][0] = -1;
  LEDMap[3][8][1] = -1;
  LEDMap[3][8][2] = -1;
  LEDMap[3][8][3] = -1;
  LEDMap[3][8][4] = -1;
  LEDMap[3][8][5] = -1;
  LEDMap[3][8][6] = -1;
  LEDMap[3][8][7] = -1;
  LEDMap[4][8][0] = -1;
  LEDMap[4][8][1] = -1;
  LEDMap[4][8][2] = -1;
  LEDMap[4][8][3] = -1;
  LEDMap[4][8][4] = -1;
  LEDMap[4][8][5] = -1;
  LEDMap[4][8][6] = -1;
  LEDMap[4][8][7] = -1;
  LEDMap[5][8][0] = -1;
  LEDMap[5][8][1] = -1;
  LEDMap[5][8][2] = -1;
  LEDMap[5][8][3] = -1;
  LEDMap[5][8][4] = -1;
  LEDMap[5][8][5] = -1;
  LEDMap[5][8][6] = -1;
  LEDMap[5][8][7] = -1;
  LEDMap[6][8][0] = -1;
  LEDMap[6][8][1] = -1;
  LEDMap[6][8][2] = -1;
  LEDMap[6][8][3] = -1;
  LEDMap[6][8][4] = -1;
  LEDMap[6][8][5] = -1;
  LEDMap[6][8][6] = -1;
  LEDMap[6][8][7] = -1;
  LEDMap[7][8][0] = 8;
  LEDMap[7][8][1] = -1;
  LEDMap[7][8][2] = -1;
  LEDMap[7][8][3] = -1;
  LEDMap[7][8][4] = -1;
  LEDMap[7][8][5] = -1;
  LEDMap[7][8][6] = -1;
  LEDMap[7][8][7] = -1;
  LEDMap[8][8][0] = 4;
  LEDMap[8][8][1] = 5;
  LEDMap[8][8][2] = 6;
  LEDMap[8][8][3] = 7;
  LEDMap[8][8][4] = -1;
  LEDMap[8][8][5] = -1;
  LEDMap[8][8][6] = -1;
  LEDMap[8][8][7] = -1;
  LEDMap[9][8][0] = 3;
  LEDMap[9][8][1] = -1;
  LEDMap[9][8][2] = -1;
  LEDMap[9][8][3] = -1;
  LEDMap[9][8][4] = -1;
  LEDMap[9][8][5] = -1;
  LEDMap[9][8][6] = -1;
  LEDMap[9][8][7] = -1;
  LEDMap[0][9][0] = 29;
  LEDMap[0][9][1] = 31;
  LEDMap[0][9][2] = 32;
  LEDMap[0][9][3] = -1;
  LEDMap[0][9][4] = -1;
  LEDMap[0][9][5] = -1;
  LEDMap[0][9][6] = -1;
  LEDMap[0][9][7] = -1;
  LEDMap[1][9][0] = 26;
  LEDMap[1][9][1] = 27;
  LEDMap[1][9][2] = 28;
  LEDMap[1][9][3] = -1;
  LEDMap[1][9][4] = -1;
  LEDMap[1][9][5] = -1;
  LEDMap[1][9][6] = -1;
  LEDMap[1][9][7] = -1;
  LEDMap[2][9][0] = 23;
  LEDMap[2][9][1] = 24;
  LEDMap[2][9][2] = 25;
  LEDMap[2][9][3] = -1;
  LEDMap[2][9][4] = -1;
  LEDMap[2][9][5] = -1;
  LEDMap[2][9][6] = -1;
  LEDMap[2][9][7] = -1;
  LEDMap[3][9][0] = 20;
  LEDMap[3][9][1] = 21;
  LEDMap[3][9][2] = 22;
  LEDMap[3][9][3] = -1;
  LEDMap[3][9][4] = -1;
  LEDMap[3][9][5] = -1;
  LEDMap[3][9][6] = -1;
  LEDMap[3][9][7] = -1;
  LEDMap[4][9][0] = 17;
  LEDMap[4][9][1] = 18;
  LEDMap[4][9][2] = 19;
  LEDMap[4][9][3] = -1;
  LEDMap[4][9][4] = -1;
  LEDMap[4][9][5] = -1;
  LEDMap[4][9][6] = -1;
  LEDMap[4][9][7] = -1;
  LEDMap[5][9][0] = 14;
  LEDMap[5][9][1] = 15;
  LEDMap[5][9][2] = 16;
  LEDMap[5][9][3] = -1;
  LEDMap[5][9][4] = -1;
  LEDMap[5][9][5] = -1;
  LEDMap[5][9][6] = -1;
  LEDMap[5][9][7] = -1;
  LEDMap[6][9][0] = 11;
  LEDMap[6][9][1] = 12;
  LEDMap[6][9][2] = 13;
  LEDMap[6][9][3] = -1;
  LEDMap[6][9][4] = -1;
  LEDMap[6][9][5] = -1;
  LEDMap[6][9][6] = -1;
  LEDMap[6][9][7] = -1;
  LEDMap[7][9][0] = 9;
  LEDMap[7][9][1] = 10;
  LEDMap[7][9][2] = -1;
  LEDMap[7][9][3] = -1;
  LEDMap[7][9][4] = -1;
  LEDMap[7][9][5] = -1;
  LEDMap[7][9][6] = -1;
  LEDMap[7][9][7] = -1;
  LEDMap[8][9][0] = -1;
  LEDMap[8][9][1] = -1;
  LEDMap[8][9][2] = -1;
  LEDMap[8][9][3] = -1;
  LEDMap[8][9][4] = -1;
  LEDMap[8][9][5] = -1;
  LEDMap[8][9][6] = -1;
  LEDMap[8][9][7] = -1;
  LEDMap[9][9][0] = -1;
  LEDMap[9][9][1] = -1;
  LEDMap[9][9][2] = -1;
  LEDMap[9][9][3] = -1;
  LEDMap[9][9][4] = -1;
  LEDMap[9][9][5] = -1;
  LEDMap[9][9][6] = -1;
  LEDMap[9][9][7] = -1;
}
