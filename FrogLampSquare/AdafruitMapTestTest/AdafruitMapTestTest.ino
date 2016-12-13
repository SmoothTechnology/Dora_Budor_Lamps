#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "MatrixDraw.h"
//#include "MatrixMapping.h"

// PIR4 Bottom
// PIR3 LEFT
// PIR1 TOP
// PIR2 RIGHT

#define DEBUG
#define LED_PIN  11
#define ANIM3

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 255

int PIR1Pin = 2; // TOP
int PIR2Pin = 3; // RIGHT
int PIR3Pin = 5; // LEFT
int PIR4Pin = 4; // Bottom

int lastPIR1State = 0;
int PIR1State = 0;

int lastPIR2State = 0;
int PIR2State = 0;

int lastPIR3State = 0;
int PIR3State = 0;

int lastPIR4State = 0;
int PIR4State = 0;

long PIR1StartTime = -100000;
long PIR2StartTime = -100000;
long PIR3StartTime = -100000;
long PIR4StartTime = -100000;

float PIR1Val = 0;
float PIR2Val = 0;
float PIR3Val = 0;
float PIR4Val = 0;
float PIRChangeValue = 0.01;

int curDefault = 0;


boolean leftUp = false;
int lastLeft = 0;

int xSize = 44;
int ySize = 34;
int maxLEDList = 2;
int LEDMap[44][34][2];
MatrixDraw draw(xSize, ySize);

// Params for width and height
const uint8_t kMatrixWidth = xSize;
const uint8_t kMatrixHeight = ySize;

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;


#define NUM_LEDS 400

#ifdef RGBW
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_RGBW + NEO_KHZ800);
#else
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

void initArray();
void DrawOneFrame(byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8);
void LightDefault(uint32_t ms);
void LightLeft(uint32_t ms);
void LightRight(uint32_t ms);
void LightUp(uint32_t ms);
void LightDown(uint32_t ms);
void TestColors(uint32_t ms);
void ClearAll();
void LightLeftSide();
void LightRightSide();
void LightUpSide();
void LightDownSide();

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

long GetPIRStartTime(int PIRState, int lastPIRState)
{
  if(PIRState == 1 && lastPIRState == 0)
    return millis();

  return -1;
}

void ReadPIRSensors()
{
	PIR1State = digitalRead(PIR1Pin);
  PIR2State = digitalRead(PIR2Pin);
	PIR3State = digitalRead(PIR3Pin);
	PIR4State = digitalRead(PIR4Pin);

  long curMillis = GetPIRStartTime(PIR1State, lastPIR1State);
  if(curMillis > 0)
    PIR1StartTime = curMillis;

  curMillis = GetPIRStartTime(PIR2State, lastPIR2State);
  if(curMillis > 0)
    PIR2StartTime = curMillis;

  curMillis = GetPIRStartTime(PIR3State, lastPIR3State);
  if(curMillis > 0)
    PIR3StartTime = curMillis;

  curMillis = GetPIRStartTime(PIR4State, lastPIR4State);
  if(curMillis > 0)
    PIR4StartTime = curMillis;

 // PIR3StartTime = 2000;

  lastPIR1State = PIR1State;
  lastPIR2State = PIR2State;
  lastPIR3State = PIR3State;
  lastPIR4State = PIR4State;
}

void setupPIR() 
{
	pinMode(PIR1Pin, INPUT);
	pinMode(PIR2Pin, INPUT);
	pinMode(PIR3Pin, INPUT);
	pinMode(PIR4Pin, INPUT);
}

void XY( uint8_t x, uint8_t y, uint8_t nHue, uint8_t degreeOfGreen = 255)
{ 
      int ledToWrite;
      for(int i = 0; i < maxLEDList; i++){
        
        ledToWrite = LEDMap[x][y][i];

        int redDefault = 100;
        int greenDefault = 140;
        int blueDefault = 51;

        redDefault = degreeOfGreen/255.0 * redDefault;
        greenDefault = degreeOfGreen/255.0 * greenDefault;
        blueDefault = degreeOfGreen/255.0 * blueDefault;

        #ifdef RGBW
        if(ledToWrite >= 0) pixels.setPixelColor(ledToWrite, pixels.Color(redDefault, greenDefault, blueDefault));
        #else
        if(ledToWrite >= 0) pixels.setPixelColor(ledToWrite, pixels.Color(greenDefault, greenDefault, blueDefault));
        #endif

        
      	//if(ledToWrite >= 0) leds[ ledToWrite ]  = CRGB( nHue, degreeOfGreen, nHue);
      	else return;
      
      }
      
  
}

void SerialDrawMatrix()
{
  for(int y = 0; y < ySize; y++)
  {
    for(int x = 0; x < xSize; x++)
    {
      int curValue = draw.GetValueAt(x,y);
      if(curValue > 0)
      {
        Serial.print(" x");
      }
      else
      {
        Serial.print("  ");
      }
    }
    Serial.println();
  }
}

void DrawMatrix()
{
  for(int x = 0; x < xSize; x++)
  {
    for(int y = 0; y < ySize; y++)
    {
      int curValue = draw.GetValueAt(x,y);
      XY(x, y, 0, curValue);
    }
  }
}


#ifdef ANIM3
//////////////////////////////////////////////////
///// BEGIN ANIMATION 5 /////////////////////////


//Demo that USES "XY" follows code below
#define STATE1TIME 0
#define STATE2TIME 300
#define STATE3TIME 600
#define STATE4TIME 900
#define STATE5TIME 1200
#define STATE6TIME 1500
#define STATE7TIME 1800
#define STATE8TIME 2100
#define STATE9TIME 2400
#define STATE10TIME 2700
#define STATE11TIME 3000
#define STATE12TIME 3300
#define STATE13TIME 3600
#define STATE14TIME 3900 // Begin stand Still
#define STATE15TIME 4200 // BEGIN Pulse
#define STATE16TIME 4500
#define STATE17TIME 4800 
#define STATE18TIME 13000 
#define STATE19TIME 13500
#define STATE20TIME 14000 // Light state to fade
#define STATE21TIME 14500
#define STATE22TIME 15000
#define STATE23TIME 15500
#define MAX_TIME 16500

#define CENTER1 0.2
#define CENTER2 0.3
#define CENTER3 0.4
#define CENTER4 0.2
#define CENTER5 0.3
#define CENTER6 0.3
#define CENTER7 0.3
#define CENTER8 0.3

#define CENTERLONG 0.35

#define CENTER9 0.3
#define CENTER10 0.3
#define CENTER11 0.4

#define MAXQ 0.5

long lastMillis = 0;

float GetPulseSpeed(int beginTime, int endTime, float maxQ, float minQ)
{
  float factor = millis() - lastMillis;
  float pulseTime = endTime - beginTime;
  return factor*((maxQ-minQ) / pulseTime);
}

// Contains Animation Information
float GetPIRValue(long PIRTime, float PIRVal)
{
  if(millis() - PIRTime > MAX_TIME)
  {
    PIRVal -= PIRChangeValue;
    if(PIRVal < 0)
      PIRVal = 0.0;
  }
  else if(millis()-PIRTime > STATE23TIME)
  {
    PIRVal = CENTER11 + 0.1*(float)random(300)/300.0;
  }
  else if(millis()-PIRTime > STATE22TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE21TIME)
  {
    PIRVal = CENTER10 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE20TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE19TIME)
  {
    PIRVal = CENTER9 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE18TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE17TIME)
  {
    PIRVal  = CENTERLONG + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE16TIME)
  {
    PIRVal  = CENTER8 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE15TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE14TIME)
  {
    PIRVal  = CENTER7 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE13TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE12TIME)
  {
    PIRVal  = CENTER6 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE11TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE10TIME)
  {
    PIRVal  = CENTER5 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE9TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE8TIME)
  {
    PIRVal  = CENTER4 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE7TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE6TIME)
  {
    PIRVal  = CENTER3 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE5TIME)
  {
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE4TIME)
  {
    PIRVal  = CENTER2 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE3TIME)
  {
    // Pulse Up
    PIRVal = 0;
  }
  else if(millis() - PIRTime > STATE2TIME)
  {
    // Pulse Down
    PIRVal  = CENTER1 + 0.1*(float)random(300)/300.0;
  }
  else if(millis() - PIRTime > STATE1TIME)
  {
    // Pulse Up
    PIRVal = 0;
  }
  
  if(PIRVal < 0)
    PIRVal = 0;
  else if(PIRVal > MAXQ)
    PIRVal = MAXQ;

  return PIRVal;
}

//////////////////////////////////////////////////
///// END ANIMATION 5 /////////////////////////
#endif

#ifdef ANIM2
//////////////////////////////////////////////////
///// BEGIN ANIMATION 2 /////////////////////////

#define STATE1TIME 0
#define STATE2TIME 1600 // END RISE UP START FLUTTER
#define STATE3TIME 1650 
#define STATE4TIME 1700
#define STATE5TIME 1800
#define STATE6TIME 1900
#define STATE7TIME 2100
#define STATE8TIME 2300
#define STATE9TIME 2700 // End Flutter one START PULSE 2
#define STATE10TIME 4100 // End PULSE 2 start flutter 2
#define STATE11TIME 4150 
#define STATE12TIME 4200
#define STATE13TIME 4300
#define STATE14TIME 4400
#define STATE15TIME 4600
#define STATE16TIME 4800
#define STATE17TIME 5200
#define STATE18TIME 5600 // End Flutter 2 Start PULSE 3
#define STATE19TIME 7200 // End PULSE 3 start Flutter 3
#define STATE20TIME 7250 
#define STATE21TIME 7300
#define STATE22TIME 7400
#define STATE23TIME 7500
#define STATE24TIME 7700
#define STATE25TIME 7900
#define STATE26TIME 8300
#define STATE27TIME 8700
#define STATE28TIME 9300 //Start the hold and tremble
#define STATE29TIME 24000 
#define MAX_TIME 28000
#define MAXQ 0.5

long lastMillis = 0;
float binaryValue = 0.3;
float lastPIRValue = 0;
float lastPIRBInary = 0;

float GetPulseSpeed(int beginTime, int endTime, float maxQ, float minQ)
{
  float factor = millis() - lastMillis;
  float pulseTime = endTime - beginTime;
  return factor*((maxQ-minQ) / pulseTime);
}

// Contains Animation Information
float GetPIRValue(long PIRTime, float PIRVal)
{
  if(millis() - PIRTime > MAX_TIME)
  {
    PIRVal -= PIRChangeValue;
    if(PIRVal < 0)
      PIRVal = 0.0;
  }
  else if(millis() - PIRTime > STATE29TIME)
  {
    PIRVal -= GetPulseSpeed(STATE29TIME, MAX_TIME, lastPIRValue, 0.2);
  }
  else if(millis() - PIRTime > STATE28TIME)
  {
    PIRVal += GetPulseSpeed(STATE28TIME, STATE29TIME, MAXQ, lastPIRBInary);
    long randNumber = random(300);
    PIRVal += ((float)randNumber/300.0)*0.1;

    lastPIRValue = PIRVal;
  }
  else if(millis() - PIRTime > STATE27TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
    lastPIRBInary = PIRVal;
  }
  else if(millis() - PIRTime > STATE26TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE25TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE24TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE23TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE22TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE21TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE20TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE19TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE18TIME)
  {
    PIRVal += GetPulseSpeed(STATE18TIME, STATE19TIME, 0.3, 0.2);
    lastPIRValue = PIRVal;
  }
  else if(millis() - PIRTime > STATE17TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE16TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE15TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE14TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE13TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE12TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE11TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE10TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE9TIME)
  {
    PIRVal += GetPulseSpeed(STATE9TIME, STATE10TIME, 0.3, 0.2);
    lastPIRValue = PIRVal;
  }
  else if(millis() - PIRTime > STATE8TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE7TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE6TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE5TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE4TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE3TIME)
  {
    PIRVal = lastPIRValue - binaryValue;
  }
  else if(millis() - PIRTime > STATE2TIME)
  {
    PIRVal = lastPIRValue + binaryValue;
  }
  else if(millis() - PIRTime > STATE1TIME)
  {
    // Pulse Up
    PIRVal += GetPulseSpeed(STATE1TIME, STATE2TIME, MAXQ, 0);
    lastPIRValue = PIRVal;
  }
  
  if(PIRVal < 0)
    PIRVal = 0;
  else if(PIRVal > MAXQ)
    PIRVal = MAXQ;

  return PIRVal;
}
//////////////////////////////////////////////////
///// END ANIMATION 2 /////////////////////////
#endif

#ifdef ANIM1
//////////////////////////////////////////////////
///// BEGIN ANIMATION 1 /////////////////////////


// Demo that USES "XY" follows code below
#define STATE1TIME 0
#define STATE2TIME 600
#define STATE3TIME 1200
#define STATE4TIME 1800
#define STATE5TIME 2400
#define STATE6TIME 2600
#define STATE7TIME 2800
#define STATE8TIME 3400
#define STATE9TIME 4000
#define STATE10TIME 4600
#define STATE11TIME 5200
#define STATE12TIME 5800
#define STATE13TIME 6400
#define STATE14TIME 7000 // Begin stand Still
#define STATE15TIME 7600 // BEGIN Pulse
#define STATE16TIME 8200 
#define STATE17TIME 8800 
#define STATE18TIME 9400
#define STATE19TIME 10000 // Light state to fade
#define STATE20TIME 13200
#define STATE21TIME 13800
#define STATE22TIME 14400
#define STATE23TIME 15000
#define STATE24TIME 18000
#define MAX_TIME 21000

#define MAXQONE 0.3
#define MAXQTWO 0.5

long lastMillis = 0;

float GetPulseSpeed(int beginTime, int endTime, float maxQ, float minQ)
{
  float factor = millis() - lastMillis;
  float pulseTime = endTime - beginTime;
  return factor*((maxQ-minQ) / pulseTime);
}

// Contains Animation Information
float GetPIRValue(long PIRTime, float PIRVal)
{
  if(millis() - PIRTime > MAX_TIME)
  {
    PIRVal -= PIRChangeValue;
    if(PIRVal < 0)
      PIRVal = 0.0;
  }
  else if(millis() - PIRTime > STATE24TIME)
  {
    PIRVal = PIRVal;
  }
  else if(millis() - PIRTime > STATE23TIME)
  {
    PIRVal += GetPulseSpeed(STATE23TIME, STATE24TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE22TIME)
  {
    PIRVal -= GetPulseSpeed(STATE22TIME, STATE23TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE21TIME)
  {
    PIRVal += GetPulseSpeed(STATE21TIME, STATE22TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE20TIME)
  {
    PIRVal -= GetPulseSpeed(STATE20TIME, STATE21TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE19TIME)
  {
    PIRVal = PIRVal;
  }
  else if(millis() - PIRTime > STATE18TIME)
  {
    PIRVal += GetPulseSpeed(STATE18TIME, STATE19TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE17TIME)
  {
    PIRVal -= GetPulseSpeed(STATE17TIME, STATE18TIME, MAXQTWO, 0);
    Serial.println("ENTER FAST");
  }
  else if(millis() - PIRTime > STATE16TIME)
  {
    PIRVal += GetPulseSpeed(STATE16TIME, STATE17TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE15TIME)
  {
    PIRVal -= GetPulseSpeed(STATE15TIME, STATE16TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE14TIME)
  {
    PIRVal += GetPulseSpeed(STATE12TIME, STATE13TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE13TIME)
  {
    PIRVal -= GetPulseSpeed(STATE13TIME, STATE14TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE12TIME)
  {
    PIRVal += GetPulseSpeed(STATE12TIME, STATE13TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE11TIME)
  {
    PIRVal -= GetPulseSpeed(STATE11TIME, STATE12TIME, MAXQTWO, 0);
  }
  else if(millis() - PIRTime > STATE10TIME)
  {
    PIRVal += GetPulseSpeed(STATE10TIME, STATE11TIME, MAXQTWO, 0);
    Serial.println("IN LARGE PULSE");
  }
  else if(millis() - PIRTime > STATE9TIME)
  {
    PIRVal += GetPulseSpeed(STATE9TIME, STATE10TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE8TIME)
  {
    PIRVal -= GetPulseSpeed(STATE8TIME, STATE9TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE7TIME)
  {
    PIRVal += GetPulseSpeed(STATE7TIME, STATE8TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE6TIME)
  {
    PIRVal -= GetPulseSpeed(STATE6TIME, STATE7TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE5TIME)
  {
    PIRVal += GetPulseSpeed(STATE5TIME, STATE6TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE4TIME)
  {
    PIRVal -= GetPulseSpeed(STATE4TIME, STATE5TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE3TIME)
  {
    // Pulse Up
    PIRVal += GetPulseSpeed(STATE3TIME, STATE4TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE2TIME)
  {
    // Pulse Down
    PIRVal -= GetPulseSpeed(STATE2TIME, STATE3TIME, MAXQONE, 0);
  }
  else if(millis() - PIRTime > STATE1TIME)
  {
    // Pulse Up
    PIRVal += GetPulseSpeed(STATE1TIME, STATE2TIME, MAXQONE, 0);
  }
  
  if(PIRVal < 0)
    PIRVal = 0;
  else if(PIRVal > MAXQTWO)
    PIRVal = MAXQTWO;

  return PIRVal;
}

//////////////////////////////////////////////////
///// END ANIMATION 1 /////////////////////////
#endif

// Can use PIR values for all lighting. They will float around based on
// The state machien described in the function above.
void GetBilinearValues(float &Q11, float &Q12, float &Q21, float &Q22)
{
  PIR1Val = GetPIRValue(PIR1StartTime, PIR1Val);
  PIR2Val = GetPIRValue(PIR2StartTime, PIR2Val);
  PIR3Val = GetPIRValue(PIR3StartTime, PIR3Val);
  PIR4Val = GetPIRValue(PIR4StartTime, PIR4Val);

  lastMillis = millis();

  Serial.print("1Val: ");
  Serial.print(PIR1Val);
  Serial.print("   2Val: ");
  Serial.print(PIR2Val);
  Serial.print("   3Val: ");
  Serial.print(PIR3Val);
  Serial.print("   4Val: ");
  Serial.println(PIR4Val);
  // Q11 - Up RIGHT
  // Q12 - Bottom Right
  // Q21 - Bottom Left
  // Q22 - Up Left
  //Q11 = (PIR2Val + PIR3Val) / 2;

  Q11 = (PIR1Val + PIR2Val) / 2;
  Q12 = (PIR4Val + PIR2Val) / 2;
  Q21 = (PIR4Val + PIR3Val) / 2;
  Q22 = (PIR1Val + PIR3Val) / 2;

}

void loop()
{
	ReadPIRSensors();

#ifdef DEBUG
	PrintPIRStates();
#endif

    uint32_t ms = millis();
//     // int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
//     // int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
//     // DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    //TestColors(ms);

    ClearAll();
    // LightDefault(ms);
    // if(PIR1State)
    // 	LightLeft(ms);
    // if(PIR2State)
    // 	LightUp(ms);
    // if(PIR3State)
    // 	LightRight(ms);
    // if(PIR4State)
    // 	LightDown(ms);
    draw.ClearMatrix();
    //draw.DrawRectangle(0,0, xSize-2, ySize-2, 100);



    float Q11 = 0.0, Q12 = 0.0, Q21 = 0.0, Q22 = 0.0;
    GetBilinearValues(Q11, Q12, Q21, Q22);
    draw.Bilinear(0, 0, xSize, ySize, Q11, Q12, Q21, Q22);

    // draw.DrawRectangle(0,0, xSize/2, ySize/2, 255);
    // draw.Fill(xSize/4, ySize/4, 255);
    // //(0,0, )


    //draw.Fill(xSize/2, ySize/2, (ms/30)%100);
    DrawMatrix();
    //SerialDrawMatrix();
    //LightDefault(ms);

   //if(PIR1State)
   // LightLeftSide();
   //if(PIR2State)
   // LightUpSide();
   //if(PIR3State)
   // LightRightSide();
   //if(PIR4State)
   // LightDownSide();
    //LightDownSide();
    //LightUpSide();

    pixels.show();
}

void ClearAll()
{
  
	for(byte y = 0; y < kMatrixHeight; y++) {
  		for(byte x = 0; x < kMatrixWidth; x++){
  			XY(x,y, 0, 0);
  	}
  }
}

void TestColors(uint32_t ms)
{
	ClearAll();

  LightDefault(ms);
 // LightLeft(ms);
  LightUp(ms);
	// if(ms < 5000){
	// 	LightRight(ms);
	// }
	// else if(ms < 7000){
	// 	LightUp(ms);
	// }
	// else if(ms < 9000){
	// 	LightLeft(ms);
	// }
	// else {
	// 	LightDown(ms);
	// }
}

void LightDefault(uint32_t ms)
{
  int pulse = 0;
  pulse = (ms/50)%255;

  if(pulse < lastLeft)
  	leftUp = !leftUp;

  lastLeft = pulse;

  if(leftUp)
  	pulse = pulse;
  else 
  	pulse = 255 - pulse;

  curDefault = pulse;

  for(byte y = 0; y < kMatrixHeight; y++) {
  	for(byte x = 0; x < kMatrixWidth; x++){
  		XY(x,y, pulse);
  	}
  }
}

void LightRight(uint32_t ms)
{
  int startPoint = 0;
  int interpDistance = 10;
  startPoint = kMatrixWidth-(ms/50)%kMatrixWidth;

  for(byte y = 0; y < kMatrixHeight; y++) {
    for(byte x = 0; x < kMatrixWidth; x++){
      if(startPoint == x)// && y == kMatrixHeight/2)
        XY(x,y, 0, 0);
      else if(abs(startPoint-x) < interpDistance){
        int interpValue = 0 + curDefault/(interpDistance-abs(startPoint-x));
        int greenVal = 0 + 255/(interpDistance - abs(startPoint-x));
        XY(x,y,interpValue, greenVal);
      }
    }
  }
}

void LightUp(uint32_t ms)
{
	  int startPoint = 0;
  int interpDistance = 10;
  startPoint = kMatrixHeight-(ms/50)%kMatrixHeight;

    for(byte y = 0; y < kMatrixHeight; y++) {
      if(startPoint == y)
      {
        for(byte x = 0; x < kMatrixWidth; x++){
          XY(x,y, 0, true);
        }
      }
      else if(abs(startPoint-y) < interpDistance)
      {
        for(byte x = 0; x < kMatrixWidth; x++){
          int interpValue = 0 + curDefault/(interpDistance-abs(startPoint-y));
          int greenVal = 0 + 255/(interpDistance - abs(startPoint-y));
          XY(x,y,interpValue, greenVal);
        }
      }
      
    }
}

void LightDown(uint32_t ms)
{
	int startPoint = 0;
  int interpDistance = 10;
	startPoint = (ms/50)%kMatrixHeight;

	  for(byte y = 0; y < kMatrixHeight; y++) {
	  	if(startPoint == y)
	  	{
	  		for(byte x = 0; x < kMatrixWidth; x++){
	  			XY(x,y, 0, true);
	  		}
	  	}
      else if(abs(startPoint-y) < interpDistance)
      {
        for(byte x = 0; x < kMatrixWidth; x++){
          int interpValue = 0 + curDefault/(interpDistance-abs(startPoint-y));
          int greenVal = 0 + 255/(interpDistance - abs(startPoint-y));
          XY(x,y,interpValue, greenVal);
        }
      }
	  	
	  }
}

void LightLeft(uint32_t ms)
{
  
  int startPoint = 0;
  int interpDistance = 10;
  startPoint = (ms/50)%kMatrixWidth;


  for(byte y = 0; y < kMatrixHeight; y++) {
  	for(byte x = 0; x < kMatrixWidth; x++){
  		if(startPoint == x)// && y == kMatrixHeight/2)
  			XY(x,y, 0, 0);
      else if(abs(startPoint-x) < interpDistance){
        int interpValue = 0 + curDefault/(interpDistance-abs(startPoint-x));
        int greenVal = 0 + 255/(interpDistance - abs(startPoint-x));
        XY(x,y,interpValue, greenVal);
      }
  	}
  }
}

void LightLeftSide()
{
  for(byte y = 0; y < kMatrixHeight; y++) {
    for(byte x = kMatrixWidth/2; x < kMatrixWidth; x++){
        XY(x,y, 0, 255);
      }
    }
}

void LightRightSide()
{
  for(byte y = 0; y < kMatrixHeight; y++) {
    for(byte x = 0; x < kMatrixWidth/2; x++){
        XY(x,y, 0, 255);
      }
    }
}

void LightUpSide()
{
  for(byte y = 0; y < kMatrixHeight/2; y++) {
    for(byte x = 0; x < kMatrixWidth; x++){
        XY(x,y, 0, 255);
      }
    }
}

void LightDownSide()
{
  for(byte y = kMatrixHeight/2; y < kMatrixHeight; y++) {
    for(byte x = 0; x < kMatrixWidth; x++){
        XY(x,y, 0, 255);
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
  
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show();
}

void initArray(){
LEDMap[0][0][0] = -1;
LEDMap[0][0][1] = -1;
LEDMap[1][0][0] = -1;
LEDMap[1][0][1] = -1;
LEDMap[2][0][0] = -1;
LEDMap[2][0][1] = -1;
LEDMap[3][0][0] = 55;
LEDMap[3][0][1] = -1;
LEDMap[4][0][0] = 56;
LEDMap[4][0][1] = -1;
LEDMap[5][0][0] = -1;
LEDMap[5][0][1] = -1;
LEDMap[6][0][0] = 57;
LEDMap[6][0][1] = -1;
LEDMap[7][0][0] = 58;
LEDMap[7][0][1] = -1;
LEDMap[8][0][0] = -1;
LEDMap[8][0][1] = -1;
LEDMap[9][0][0] = 59;
LEDMap[9][0][1] = -1;
LEDMap[10][0][0] = -1;
LEDMap[10][0][1] = -1;
LEDMap[11][0][0] = -1;
LEDMap[11][0][1] = -1;
LEDMap[12][0][0] = 61;
LEDMap[12][0][1] = -1;
LEDMap[13][0][0] = 62;
LEDMap[13][0][1] = -1;
LEDMap[14][0][0] = -1;
LEDMap[14][0][1] = -1;
LEDMap[15][0][0] = 63;
LEDMap[15][0][1] = -1;
LEDMap[16][0][0] = -1;
LEDMap[16][0][1] = -1;
LEDMap[17][0][0] = 64;
LEDMap[17][0][1] = -1;
LEDMap[18][0][0] = 65;
LEDMap[18][0][1] = -1;
LEDMap[19][0][0] = -1;
LEDMap[19][0][1] = -1;
LEDMap[20][0][0] = 66;
LEDMap[20][0][1] = -1;
LEDMap[21][0][0] = 67;
LEDMap[21][0][1] = -1;
LEDMap[22][0][0] = -1;
LEDMap[22][0][1] = -1;
LEDMap[23][0][0] = 68;
LEDMap[23][0][1] = -1;
LEDMap[24][0][0] = 69;
LEDMap[24][0][1] = -1;
LEDMap[25][0][0] = -1;
LEDMap[25][0][1] = -1;
LEDMap[26][0][0] = 70;
LEDMap[26][0][1] = -1;
LEDMap[27][0][0] = 71;
LEDMap[27][0][1] = -1;
LEDMap[28][0][0] = -1;
LEDMap[28][0][1] = -1;
LEDMap[29][0][0] = 72;
LEDMap[29][0][1] = -1;
LEDMap[30][0][0] = 73;
LEDMap[30][0][1] = -1;
LEDMap[31][0][0] = -1;
LEDMap[31][0][1] = -1;
LEDMap[32][0][0] = 74;
LEDMap[32][0][1] = -1;
LEDMap[33][0][0] = -1;
LEDMap[33][0][1] = -1;
LEDMap[34][0][0] = -1;
LEDMap[34][0][1] = -1;
LEDMap[35][0][0] = -1;
LEDMap[35][0][1] = -1;
LEDMap[36][0][0] = -1;
LEDMap[36][0][1] = -1;
LEDMap[37][0][0] = -1;
LEDMap[37][0][1] = -1;
LEDMap[38][0][0] = -1;
LEDMap[38][0][1] = -1;
LEDMap[39][0][0] = -1;
LEDMap[39][0][1] = -1;
LEDMap[40][0][0] = -1;
LEDMap[40][0][1] = -1;
LEDMap[41][0][0] = -1;
LEDMap[41][0][1] = -1;
LEDMap[42][0][0] = -1;
LEDMap[42][0][1] = -1;
LEDMap[43][0][0] = -1;
LEDMap[43][0][1] = -1;
LEDMap[0][1][0] = -1;
LEDMap[0][1][1] = -1;
LEDMap[1][1][0] = 53;
LEDMap[1][1][1] = 54;
LEDMap[2][1][0] = -1;
LEDMap[2][1][1] = -1;
LEDMap[3][1][0] = -1;
LEDMap[3][1][1] = -1;
LEDMap[4][1][0] = -1;
LEDMap[4][1][1] = -1;
LEDMap[5][1][0] = -1;
LEDMap[5][1][1] = -1;
LEDMap[6][1][0] = -1;
LEDMap[6][1][1] = -1;
LEDMap[7][1][0] = -1;
LEDMap[7][1][1] = -1;
LEDMap[8][1][0] = -1;
LEDMap[8][1][1] = -1;
LEDMap[9][1][0] = -1;
LEDMap[9][1][1] = -1;
LEDMap[10][1][0] = -1;
LEDMap[10][1][1] = -1;
LEDMap[11][1][0] = -1;
LEDMap[11][1][1] = -1;
LEDMap[12][1][0] = -1;
LEDMap[12][1][1] = -1;
LEDMap[13][1][0] = -1;
LEDMap[13][1][1] = -1;
LEDMap[14][1][0] = -1;
LEDMap[14][1][1] = -1;
LEDMap[15][1][0] = -1;
LEDMap[15][1][1] = -1;
LEDMap[16][1][0] = -1;
LEDMap[16][1][1] = -1;
LEDMap[17][1][0] = -1;
LEDMap[17][1][1] = -1;
LEDMap[18][1][0] = -1;
LEDMap[18][1][1] = -1;
LEDMap[19][1][0] = -1;
LEDMap[19][1][1] = -1;
LEDMap[20][1][0] = -1;
LEDMap[20][1][1] = -1;
LEDMap[21][1][0] = -1;
LEDMap[21][1][1] = -1;
LEDMap[22][1][0] = -1;
LEDMap[22][1][1] = -1;
LEDMap[23][1][0] = -1;
LEDMap[23][1][1] = -1;
LEDMap[24][1][0] = -1;
LEDMap[24][1][1] = -1;
LEDMap[25][1][0] = -1;
LEDMap[25][1][1] = -1;
LEDMap[26][1][0] = -1;
LEDMap[26][1][1] = -1;
LEDMap[27][1][0] = -1;
LEDMap[27][1][1] = -1;
LEDMap[28][1][0] = -1;
LEDMap[28][1][1] = -1;
LEDMap[29][1][0] = -1;
LEDMap[29][1][1] = -1;
LEDMap[30][1][0] = -1;
LEDMap[30][1][1] = -1;
LEDMap[31][1][0] = -1;
LEDMap[31][1][1] = -1;
LEDMap[32][1][0] = -1;
LEDMap[32][1][1] = -1;
LEDMap[33][1][0] = 75;
LEDMap[33][1][1] = -1;
LEDMap[34][1][0] = 76;
LEDMap[34][1][1] = -1;
LEDMap[35][1][0] = -1;
LEDMap[35][1][1] = -1;
LEDMap[36][1][0] = 77;
LEDMap[36][1][1] = -1;
LEDMap[37][1][0] = 78;
LEDMap[37][1][1] = -1;
LEDMap[38][1][0] = -1;
LEDMap[38][1][1] = -1;
LEDMap[39][1][0] = -1;
LEDMap[39][1][1] = -1;
LEDMap[40][1][0] = -1;
LEDMap[40][1][1] = -1;
LEDMap[41][1][0] = -1;
LEDMap[41][1][1] = -1;
LEDMap[42][1][0] = -1;
LEDMap[42][1][1] = -1;
LEDMap[43][1][0] = -1;
LEDMap[43][1][1] = -1;
LEDMap[0][2][0] = 52;
LEDMap[0][2][1] = -1;
LEDMap[1][2][0] = -1;
LEDMap[1][2][1] = -1;
LEDMap[2][2][0] = -1;
LEDMap[2][2][1] = -1;
LEDMap[3][2][0] = -1;
LEDMap[3][2][1] = -1;
LEDMap[4][2][0] = -1;
LEDMap[4][2][1] = -1;
LEDMap[5][2][0] = -1;
LEDMap[5][2][1] = -1;
LEDMap[6][2][0] = -1;
LEDMap[6][2][1] = -1;
LEDMap[7][2][0] = -1;
LEDMap[7][2][1] = -1;
LEDMap[8][2][0] = -1;
LEDMap[8][2][1] = -1;
LEDMap[9][2][0] = -1;
LEDMap[9][2][1] = -1;
LEDMap[10][2][0] = -1;
LEDMap[10][2][1] = -1;
LEDMap[11][2][0] = -1;
LEDMap[11][2][1] = -1;
LEDMap[12][2][0] = -1;
LEDMap[12][2][1] = -1;
LEDMap[13][2][0] = -1;
LEDMap[13][2][1] = -1;
LEDMap[14][2][0] = -1;
LEDMap[14][2][1] = -1;
LEDMap[15][2][0] = -1;
LEDMap[15][2][1] = -1;
LEDMap[16][2][0] = -1;
LEDMap[16][2][1] = -1;
LEDMap[17][2][0] = -1;
LEDMap[17][2][1] = -1;
LEDMap[18][2][0] = -1;
LEDMap[18][2][1] = -1;
LEDMap[19][2][0] = -1;
LEDMap[19][2][1] = -1;
LEDMap[20][2][0] = -1;
LEDMap[20][2][1] = -1;
LEDMap[21][2][0] = -1;
LEDMap[21][2][1] = -1;
LEDMap[22][2][0] = -1;
LEDMap[22][2][1] = -1;
LEDMap[23][2][0] = -1;
LEDMap[23][2][1] = -1;
LEDMap[24][2][0] = -1;
LEDMap[24][2][1] = -1;
LEDMap[25][2][0] = -1;
LEDMap[25][2][1] = -1;
LEDMap[26][2][0] = -1;
LEDMap[26][2][1] = -1;
LEDMap[27][2][0] = -1;
LEDMap[27][2][1] = -1;
LEDMap[28][2][0] = -1;
LEDMap[28][2][1] = -1;
LEDMap[29][2][0] = -1;
LEDMap[29][2][1] = -1;
LEDMap[30][2][0] = -1;
LEDMap[30][2][1] = -1;
LEDMap[31][2][0] = -1;
LEDMap[31][2][1] = -1;
LEDMap[32][2][0] = -1;
LEDMap[32][2][1] = -1;
LEDMap[33][2][0] = -1;
LEDMap[33][2][1] = -1;
LEDMap[34][2][0] = -1;
LEDMap[34][2][1] = -1;
LEDMap[35][2][0] = -1;
LEDMap[35][2][1] = -1;
LEDMap[36][2][0] = -1;
LEDMap[36][2][1] = -1;
LEDMap[37][2][0] = -1;
LEDMap[37][2][1] = -1;
LEDMap[38][2][0] = -1;
LEDMap[38][2][1] = -1;
LEDMap[39][2][0] = 79;
LEDMap[39][2][1] = -1;
LEDMap[40][2][0] = -1;
LEDMap[40][2][1] = -1;
LEDMap[41][2][0] = -1;
LEDMap[41][2][1] = -1;
LEDMap[42][2][0] = -1;
LEDMap[42][2][1] = -1;
LEDMap[43][2][0] = -1;
LEDMap[43][2][1] = -1;
LEDMap[0][3][0] = -1;
LEDMap[0][3][1] = -1;
LEDMap[1][3][0] = -1;
LEDMap[1][3][1] = -1;
LEDMap[2][3][0] = -1;
LEDMap[2][3][1] = -1;
LEDMap[3][3][0] = -1;
LEDMap[3][3][1] = -1;
LEDMap[4][3][0] = -1;
LEDMap[4][3][1] = -1;
LEDMap[5][3][0] = -1;
LEDMap[5][3][1] = -1;
LEDMap[6][3][0] = -1;
LEDMap[6][3][1] = -1;
LEDMap[7][3][0] = -1;
LEDMap[7][3][1] = -1;
LEDMap[8][3][0] = -1;
LEDMap[8][3][1] = -1;
LEDMap[9][3][0] = -1;
LEDMap[9][3][1] = -1;
LEDMap[10][3][0] = -1;
LEDMap[10][3][1] = -1;
LEDMap[11][3][0] = -1;
LEDMap[11][3][1] = -1;
LEDMap[12][3][0] = -1;
LEDMap[12][3][1] = -1;
LEDMap[13][3][0] = -1;
LEDMap[13][3][1] = -1;
LEDMap[14][3][0] = -1;
LEDMap[14][3][1] = -1;
LEDMap[15][3][0] = -1;
LEDMap[15][3][1] = -1;
LEDMap[16][3][0] = -1;
LEDMap[16][3][1] = -1;
LEDMap[17][3][0] = -1;
LEDMap[17][3][1] = -1;
LEDMap[18][3][0] = -1;
LEDMap[18][3][1] = -1;
LEDMap[19][3][0] = -1;
LEDMap[19][3][1] = -1;
LEDMap[20][3][0] = -1;
LEDMap[20][3][1] = -1;
LEDMap[21][3][0] = -1;
LEDMap[21][3][1] = -1;
LEDMap[22][3][0] = -1;
LEDMap[22][3][1] = -1;
LEDMap[23][3][0] = -1;
LEDMap[23][3][1] = -1;
LEDMap[24][3][0] = -1;
LEDMap[24][3][1] = -1;
LEDMap[25][3][0] = -1;
LEDMap[25][3][1] = -1;
LEDMap[26][3][0] = -1;
LEDMap[26][3][1] = -1;
LEDMap[27][3][0] = -1;
LEDMap[27][3][1] = -1;
LEDMap[28][3][0] = -1;
LEDMap[28][3][1] = -1;
LEDMap[29][3][0] = -1;
LEDMap[29][3][1] = -1;
LEDMap[30][3][0] = -1;
LEDMap[30][3][1] = -1;
LEDMap[31][3][0] = -1;
LEDMap[31][3][1] = -1;
LEDMap[32][3][0] = -1;
LEDMap[32][3][1] = -1;
LEDMap[33][3][0] = -1;
LEDMap[33][3][1] = -1;
LEDMap[34][3][0] = -1;
LEDMap[34][3][1] = -1;
LEDMap[35][3][0] = -1;
LEDMap[35][3][1] = -1;
LEDMap[36][3][0] = -1;
LEDMap[36][3][1] = -1;
LEDMap[37][3][0] = -1;
LEDMap[37][3][1] = -1;
LEDMap[38][3][0] = -1;
LEDMap[38][3][1] = -1;
LEDMap[39][3][0] = -1;
LEDMap[39][3][1] = -1;
LEDMap[40][3][0] = 80;
LEDMap[40][3][1] = 81;
LEDMap[41][3][0] = -1;
LEDMap[41][3][1] = -1;
LEDMap[42][3][0] = -1;
LEDMap[42][3][1] = -1;
LEDMap[43][3][0] = -1;
LEDMap[43][3][1] = -1;
LEDMap[0][4][0] = 51;
LEDMap[0][4][1] = -1;
LEDMap[1][4][0] = -1;
LEDMap[1][4][1] = -1;
LEDMap[2][4][0] = -1;
LEDMap[2][4][1] = -1;
LEDMap[3][4][0] = -1;
LEDMap[3][4][1] = -1;
LEDMap[4][4][0] = -1;
LEDMap[4][4][1] = -1;
LEDMap[5][4][0] = -1;
LEDMap[5][4][1] = -1;
LEDMap[6][4][0] = -1;
LEDMap[6][4][1] = -1;
LEDMap[7][4][0] = -1;
LEDMap[7][4][1] = -1;
LEDMap[8][4][0] = -1;
LEDMap[8][4][1] = -1;
LEDMap[9][4][0] = -1;
LEDMap[9][4][1] = -1;
LEDMap[10][4][0] = -1;
LEDMap[10][4][1] = -1;
LEDMap[11][4][0] = -1;
LEDMap[11][4][1] = -1;
LEDMap[12][4][0] = -1;
LEDMap[12][4][1] = -1;
LEDMap[13][4][0] = -1;
LEDMap[13][4][1] = -1;
LEDMap[14][4][0] = -1;
LEDMap[14][4][1] = -1;
LEDMap[15][4][0] = -1;
LEDMap[15][4][1] = -1;
LEDMap[16][4][0] = -1;
LEDMap[16][4][1] = -1;
LEDMap[17][4][0] = -1;
LEDMap[17][4][1] = -1;
LEDMap[18][4][0] = -1;
LEDMap[18][4][1] = -1;
LEDMap[19][4][0] = -1;
LEDMap[19][4][1] = -1;
LEDMap[20][4][0] = -1;
LEDMap[20][4][1] = -1;
LEDMap[21][4][0] = -1;
LEDMap[21][4][1] = -1;
LEDMap[22][4][0] = -1;
LEDMap[22][4][1] = -1;
LEDMap[23][4][0] = -1;
LEDMap[23][4][1] = -1;
LEDMap[24][4][0] = -1;
LEDMap[24][4][1] = -1;
LEDMap[25][4][0] = -1;
LEDMap[25][4][1] = -1;
LEDMap[26][4][0] = -1;
LEDMap[26][4][1] = -1;
LEDMap[27][4][0] = -1;
LEDMap[27][4][1] = -1;
LEDMap[28][4][0] = -1;
LEDMap[28][4][1] = -1;
LEDMap[29][4][0] = -1;
LEDMap[29][4][1] = -1;
LEDMap[30][4][0] = -1;
LEDMap[30][4][1] = -1;
LEDMap[31][4][0] = -1;
LEDMap[31][4][1] = -1;
LEDMap[32][4][0] = -1;
LEDMap[32][4][1] = -1;
LEDMap[33][4][0] = -1;
LEDMap[33][4][1] = -1;
LEDMap[34][4][0] = -1;
LEDMap[34][4][1] = -1;
LEDMap[35][4][0] = -1;
LEDMap[35][4][1] = -1;
LEDMap[36][4][0] = -1;
LEDMap[36][4][1] = -1;
LEDMap[37][4][0] = -1;
LEDMap[37][4][1] = -1;
LEDMap[38][4][0] = -1;
LEDMap[38][4][1] = -1;
LEDMap[39][4][0] = -1;
LEDMap[39][4][1] = -1;
LEDMap[40][4][0] = -1;
LEDMap[40][4][1] = -1;
LEDMap[41][4][0] = -1;
LEDMap[41][4][1] = -1;
LEDMap[42][4][0] = -1;
LEDMap[42][4][1] = -1;
LEDMap[43][4][0] = -1;
LEDMap[43][4][1] = -1;
LEDMap[0][5][0] = -1;
LEDMap[0][5][1] = -1;
LEDMap[1][5][0] = -1;
LEDMap[1][5][1] = -1;
LEDMap[2][5][0] = -1;
LEDMap[2][5][1] = -1;
LEDMap[3][5][0] = -1;
LEDMap[3][5][1] = -1;
LEDMap[4][5][0] = -1;
LEDMap[4][5][1] = -1;
LEDMap[5][5][0] = -1;
LEDMap[5][5][1] = -1;
LEDMap[6][5][0] = 136;
LEDMap[6][5][1] = -1;
LEDMap[7][5][0] = 137;
LEDMap[7][5][1] = -1;
LEDMap[8][5][0] = -1;
LEDMap[8][5][1] = -1;
LEDMap[9][5][0] = 138;
LEDMap[9][5][1] = -1;
LEDMap[10][5][0] = 139;
LEDMap[10][5][1] = -1;
LEDMap[11][5][0] = -1;
LEDMap[11][5][1] = -1;
LEDMap[12][5][0] = 140;
LEDMap[12][5][1] = -1;
LEDMap[13][5][0] = 141;
LEDMap[13][5][1] = -1;
LEDMap[14][5][0] = 142;
LEDMap[14][5][1] = -1;
LEDMap[15][5][0] = -1;
LEDMap[15][5][1] = -1;
LEDMap[16][5][0] = 143;
LEDMap[16][5][1] = -1;
LEDMap[17][5][0] = -1;
LEDMap[17][5][1] = -1;
LEDMap[18][5][0] = 144;
LEDMap[18][5][1] = -1;
LEDMap[19][5][0] = 145;
LEDMap[19][5][1] = -1;
LEDMap[20][5][0] = -1;
LEDMap[20][5][1] = -1;
LEDMap[21][5][0] = 146;
LEDMap[21][5][1] = -1;
LEDMap[22][5][0] = 147;
LEDMap[22][5][1] = -1;
LEDMap[23][5][0] = 148;
LEDMap[23][5][1] = -1;
LEDMap[24][5][0] = -1;
LEDMap[24][5][1] = -1;
LEDMap[25][5][0] = 149;
LEDMap[25][5][1] = -1;
LEDMap[26][5][0] = -1;
LEDMap[26][5][1] = -1;
LEDMap[27][5][0] = 150;
LEDMap[27][5][1] = -1;
LEDMap[28][5][0] = 151;
LEDMap[28][5][1] = -1;
LEDMap[29][5][0] = 152;
LEDMap[29][5][1] = -1;
LEDMap[30][5][0] = -1;
LEDMap[30][5][1] = -1;
LEDMap[31][5][0] = 153;
LEDMap[31][5][1] = -1;
LEDMap[32][5][0] = 154;
LEDMap[32][5][1] = -1;
LEDMap[33][5][0] = -1;
LEDMap[33][5][1] = -1;
LEDMap[34][5][0] = -1;
LEDMap[34][5][1] = -1;
LEDMap[35][5][0] = -1;
LEDMap[35][5][1] = -1;
LEDMap[36][5][0] = -1;
LEDMap[36][5][1] = -1;
LEDMap[37][5][0] = -1;
LEDMap[37][5][1] = -1;
LEDMap[38][5][0] = -1;
LEDMap[38][5][1] = -1;
LEDMap[39][5][0] = -1;
LEDMap[39][5][1] = -1;
LEDMap[40][5][0] = -1;
LEDMap[40][5][1] = -1;
LEDMap[41][5][0] = 82;
LEDMap[41][5][1] = -1;
LEDMap[42][5][0] = -1;
LEDMap[42][5][1] = -1;
LEDMap[43][5][0] = -1;
LEDMap[43][5][1] = -1;
LEDMap[0][6][0] = 50;
LEDMap[0][6][1] = -1;
LEDMap[1][6][0] = -1;
LEDMap[1][6][1] = -1;
LEDMap[2][6][0] = -1;
LEDMap[2][6][1] = -1;
LEDMap[3][6][0] = -1;
LEDMap[3][6][1] = -1;
LEDMap[4][6][0] = -1;
LEDMap[4][6][1] = -1;
LEDMap[5][6][0] = -1;
LEDMap[5][6][1] = -1;
LEDMap[6][6][0] = -1;
LEDMap[6][6][1] = -1;
LEDMap[7][6][0] = -1;
LEDMap[7][6][1] = -1;
LEDMap[8][6][0] = -1;
LEDMap[8][6][1] = -1;
LEDMap[9][6][0] = -1;
LEDMap[9][6][1] = -1;
LEDMap[10][6][0] = -1;
LEDMap[10][6][1] = -1;
LEDMap[11][6][0] = -1;
LEDMap[11][6][1] = -1;
LEDMap[12][6][0] = -1;
LEDMap[12][6][1] = -1;
LEDMap[13][6][0] = -1;
LEDMap[13][6][1] = -1;
LEDMap[14][6][0] = -1;
LEDMap[14][6][1] = -1;
LEDMap[15][6][0] = -1;
LEDMap[15][6][1] = -1;
LEDMap[16][6][0] = -1;
LEDMap[16][6][1] = -1;
LEDMap[17][6][0] = -1;
LEDMap[17][6][1] = -1;
LEDMap[18][6][0] = -1;
LEDMap[18][6][1] = -1;
LEDMap[19][6][0] = -1;
LEDMap[19][6][1] = -1;
LEDMap[20][6][0] = -1;
LEDMap[20][6][1] = -1;
LEDMap[21][6][0] = -1;
LEDMap[21][6][1] = -1;
LEDMap[22][6][0] = -1;
LEDMap[22][6][1] = -1;
LEDMap[23][6][0] = -1;
LEDMap[23][6][1] = -1;
LEDMap[24][6][0] = -1;
LEDMap[24][6][1] = -1;
LEDMap[25][6][0] = -1;
LEDMap[25][6][1] = -1;
LEDMap[26][6][0] = -1;
LEDMap[26][6][1] = -1;
LEDMap[27][6][0] = -1;
LEDMap[27][6][1] = -1;
LEDMap[28][6][0] = -1;
LEDMap[28][6][1] = -1;
LEDMap[29][6][0] = -1;
LEDMap[29][6][1] = -1;
LEDMap[30][6][0] = -1;
LEDMap[30][6][1] = -1;
LEDMap[31][6][0] = -1;
LEDMap[31][6][1] = -1;
LEDMap[32][6][0] = -1;
LEDMap[32][6][1] = -1;
LEDMap[33][6][0] = -1;
LEDMap[33][6][1] = -1;
LEDMap[34][6][0] = 155;
LEDMap[34][6][1] = -1;
LEDMap[35][6][0] = 156;
LEDMap[35][6][1] = -1;
LEDMap[36][6][0] = -1;
LEDMap[36][6][1] = -1;
LEDMap[37][6][0] = 157;
LEDMap[37][6][1] = -1;
LEDMap[38][6][0] = -1;
LEDMap[38][6][1] = -1;
LEDMap[39][6][0] = -1;
LEDMap[39][6][1] = -1;
LEDMap[40][6][0] = -1;
LEDMap[40][6][1] = -1;
LEDMap[41][6][0] = -1;
LEDMap[41][6][1] = -1;
LEDMap[42][6][0] = 83;
LEDMap[42][6][1] = -1;
LEDMap[43][6][0] = -1;
LEDMap[43][6][1] = -1;
LEDMap[0][7][0] = 49;
LEDMap[0][7][1] = -1;
LEDMap[1][7][0] = -1;
LEDMap[1][7][1] = -1;
LEDMap[2][7][0] = -1;
LEDMap[2][7][1] = -1;
LEDMap[3][7][0] = -1;
LEDMap[3][7][1] = -1;
LEDMap[4][7][0] = -1;
LEDMap[4][7][1] = -1;
LEDMap[5][7][0] = 135;
LEDMap[5][7][1] = -1;
LEDMap[6][7][0] = -1;
LEDMap[6][7][1] = -1;
LEDMap[7][7][0] = -1;
LEDMap[7][7][1] = -1;
LEDMap[8][7][0] = -1;
LEDMap[8][7][1] = -1;
LEDMap[9][7][0] = -1;
LEDMap[9][7][1] = -1;
LEDMap[10][7][0] = -1;
LEDMap[10][7][1] = -1;
LEDMap[11][7][0] = -1;
LEDMap[11][7][1] = -1;
LEDMap[12][7][0] = -1;
LEDMap[12][7][1] = -1;
LEDMap[13][7][0] = -1;
LEDMap[13][7][1] = -1;
LEDMap[14][7][0] = -1;
LEDMap[14][7][1] = -1;
LEDMap[15][7][0] = -1;
LEDMap[15][7][1] = -1;
LEDMap[16][7][0] = -1;
LEDMap[16][7][1] = -1;
LEDMap[17][7][0] = -1;
LEDMap[17][7][1] = -1;
LEDMap[18][7][0] = -1;
LEDMap[18][7][1] = -1;
LEDMap[19][7][0] = -1;
LEDMap[19][7][1] = -1;
LEDMap[20][7][0] = -1;
LEDMap[20][7][1] = -1;
LEDMap[21][7][0] = -1;
LEDMap[21][7][1] = -1;
LEDMap[22][7][0] = -1;
LEDMap[22][7][1] = -1;
LEDMap[23][7][0] = -1;
LEDMap[23][7][1] = -1;
LEDMap[24][7][0] = -1;
LEDMap[24][7][1] = -1;
LEDMap[25][7][0] = -1;
LEDMap[25][7][1] = -1;
LEDMap[26][7][0] = -1;
LEDMap[26][7][1] = -1;
LEDMap[27][7][0] = -1;
LEDMap[27][7][1] = -1;
LEDMap[28][7][0] = -1;
LEDMap[28][7][1] = -1;
LEDMap[29][7][0] = -1;
LEDMap[29][7][1] = -1;
LEDMap[30][7][0] = -1;
LEDMap[30][7][1] = -1;
LEDMap[31][7][0] = -1;
LEDMap[31][7][1] = -1;
LEDMap[32][7][0] = -1;
LEDMap[32][7][1] = -1;
LEDMap[33][7][0] = -1;
LEDMap[33][7][1] = -1;
LEDMap[34][7][0] = -1;
LEDMap[34][7][1] = -1;
LEDMap[35][7][0] = -1;
LEDMap[35][7][1] = -1;
LEDMap[36][7][0] = -1;
LEDMap[36][7][1] = -1;
LEDMap[37][7][0] = -1;
LEDMap[37][7][1] = -1;
LEDMap[38][7][0] = 158;
LEDMap[38][7][1] = 159;
LEDMap[39][7][0] = -1;
LEDMap[39][7][1] = -1;
LEDMap[40][7][0] = -1;
LEDMap[40][7][1] = -1;
LEDMap[41][7][0] = -1;
LEDMap[41][7][1] = -1;
LEDMap[42][7][0] = 84;
LEDMap[42][7][1] = -1;
LEDMap[43][7][0] = -1;
LEDMap[43][7][1] = -1;
LEDMap[0][8][0] = -1;
LEDMap[0][8][1] = -1;
LEDMap[1][8][0] = -1;
LEDMap[1][8][1] = -1;
LEDMap[2][8][0] = -1;
LEDMap[2][8][1] = -1;
LEDMap[3][8][0] = -1;
LEDMap[3][8][1] = -1;
LEDMap[4][8][0] = -1;
LEDMap[4][8][1] = -1;
LEDMap[5][8][0] = 134;
LEDMap[5][8][1] = -1;
LEDMap[6][8][0] = -1;
LEDMap[6][8][1] = -1;
LEDMap[7][8][0] = -1;
LEDMap[7][8][1] = -1;
LEDMap[8][8][0] = -1;
LEDMap[8][8][1] = -1;
LEDMap[9][8][0] = -1;
LEDMap[9][8][1] = -1;
LEDMap[10][8][0] = -1;
LEDMap[10][8][1] = -1;
LEDMap[11][8][0] = -1;
LEDMap[11][8][1] = -1;
LEDMap[12][8][0] = -1;
LEDMap[12][8][1] = -1;
LEDMap[13][8][0] = -1;
LEDMap[13][8][1] = -1;
LEDMap[14][8][0] = -1;
LEDMap[14][8][1] = -1;
LEDMap[15][8][0] = -1;
LEDMap[15][8][1] = -1;
LEDMap[16][8][0] = -1;
LEDMap[16][8][1] = -1;
LEDMap[17][8][0] = -1;
LEDMap[17][8][1] = -1;
LEDMap[18][8][0] = -1;
LEDMap[18][8][1] = -1;
LEDMap[19][8][0] = -1;
LEDMap[19][8][1] = -1;
LEDMap[20][8][0] = -1;
LEDMap[20][8][1] = -1;
LEDMap[21][8][0] = -1;
LEDMap[21][8][1] = -1;
LEDMap[22][8][0] = -1;
LEDMap[22][8][1] = -1;
LEDMap[23][8][0] = -1;
LEDMap[23][8][1] = -1;
LEDMap[24][8][0] = -1;
LEDMap[24][8][1] = -1;
LEDMap[25][8][0] = -1;
LEDMap[25][8][1] = -1;
LEDMap[26][8][0] = -1;
LEDMap[26][8][1] = -1;
LEDMap[27][8][0] = -1;
LEDMap[27][8][1] = -1;
LEDMap[28][8][0] = -1;
LEDMap[28][8][1] = -1;
LEDMap[29][8][0] = -1;
LEDMap[29][8][1] = -1;
LEDMap[30][8][0] = -1;
LEDMap[30][8][1] = -1;
LEDMap[31][8][0] = -1;
LEDMap[31][8][1] = -1;
LEDMap[32][8][0] = -1;
LEDMap[32][8][1] = -1;
LEDMap[33][8][0] = -1;
LEDMap[33][8][1] = -1;
LEDMap[34][8][0] = -1;
LEDMap[34][8][1] = -1;
LEDMap[35][8][0] = -1;
LEDMap[35][8][1] = -1;
LEDMap[36][8][0] = -1;
LEDMap[36][8][1] = -1;
LEDMap[37][8][0] = -1;
LEDMap[37][8][1] = -1;
LEDMap[38][8][0] = -1;
LEDMap[38][8][1] = -1;
LEDMap[39][8][0] = 160;
LEDMap[39][8][1] = -1;
LEDMap[40][8][0] = -1;
LEDMap[40][8][1] = -1;
LEDMap[41][8][0] = -1;
LEDMap[41][8][1] = -1;
LEDMap[42][8][0] = 85;
LEDMap[42][8][1] = -1;
LEDMap[43][8][0] = -1;
LEDMap[43][8][1] = -1;
LEDMap[0][9][0] = 48;
LEDMap[0][9][1] = -1;
LEDMap[1][9][0] = -1;
LEDMap[1][9][1] = -1;
LEDMap[2][9][0] = -1;
LEDMap[2][9][1] = -1;
LEDMap[3][9][0] = -1;
LEDMap[3][9][1] = -1;
LEDMap[4][9][0] = -1;
LEDMap[4][9][1] = -1;
LEDMap[5][9][0] = -1;
LEDMap[5][9][1] = -1;
LEDMap[6][9][0] = -1;
LEDMap[6][9][1] = -1;
LEDMap[7][9][0] = -1;
LEDMap[7][9][1] = -1;
LEDMap[8][9][0] = -1;
LEDMap[8][9][1] = -1;
LEDMap[9][9][0] = -1;
LEDMap[9][9][1] = -1;
LEDMap[10][9][0] = -1;
LEDMap[10][9][1] = -1;
LEDMap[11][9][0] = -1;
LEDMap[11][9][1] = -1;
LEDMap[12][9][0] = 199;
LEDMap[12][9][1] = -1;
LEDMap[13][9][0] = 200;
LEDMap[13][9][1] = -1;
LEDMap[14][9][0] = -1;
LEDMap[14][9][1] = -1;
LEDMap[15][9][0] = 201;
LEDMap[15][9][1] = -1;
LEDMap[16][9][0] = 202;
LEDMap[16][9][1] = -1;
LEDMap[17][9][0] = -1;
LEDMap[17][9][1] = -1;
LEDMap[18][9][0] = 203;
LEDMap[18][9][1] = -1;
LEDMap[19][9][0] = 204;
LEDMap[19][9][1] = -1;
LEDMap[20][9][0] = -1;
LEDMap[20][9][1] = -1;
LEDMap[21][9][0] = 205;
LEDMap[21][9][1] = -1;
LEDMap[22][9][0] = 206;
LEDMap[22][9][1] = -1;
LEDMap[23][9][0] = -1;
LEDMap[23][9][1] = -1;
LEDMap[24][9][0] = 207;
LEDMap[24][9][1] = -1;
LEDMap[25][9][0] = -1;
LEDMap[25][9][1] = -1;
LEDMap[26][9][0] = -1;
LEDMap[26][9][1] = -1;
LEDMap[27][9][0] = -1;
LEDMap[27][9][1] = -1;
LEDMap[28][9][0] = -1;
LEDMap[28][9][1] = -1;
LEDMap[29][9][0] = -1;
LEDMap[29][9][1] = -1;
LEDMap[30][9][0] = -1;
LEDMap[30][9][1] = -1;
LEDMap[31][9][0] = -1;
LEDMap[31][9][1] = -1;
LEDMap[32][9][0] = -1;
LEDMap[32][9][1] = -1;
LEDMap[33][9][0] = -1;
LEDMap[33][9][1] = -1;
LEDMap[34][9][0] = -1;
LEDMap[34][9][1] = -1;
LEDMap[35][9][0] = -1;
LEDMap[35][9][1] = -1;
LEDMap[36][9][0] = -1;
LEDMap[36][9][1] = -1;
LEDMap[37][9][0] = -1;
LEDMap[37][9][1] = -1;
LEDMap[38][9][0] = -1;
LEDMap[38][9][1] = -1;
LEDMap[39][9][0] = -1;
LEDMap[39][9][1] = -1;
LEDMap[40][9][0] = -1;
LEDMap[40][9][1] = -1;
LEDMap[41][9][0] = -1;
LEDMap[41][9][1] = -1;
LEDMap[42][9][0] = -1;
LEDMap[42][9][1] = -1;
LEDMap[43][9][0] = -1;
LEDMap[43][9][1] = -1;
LEDMap[0][10][0] = -1;
LEDMap[0][10][1] = -1;
LEDMap[1][10][0] = -1;
LEDMap[1][10][1] = -1;
LEDMap[2][10][0] = -1;
LEDMap[2][10][1] = -1;
LEDMap[3][10][0] = -1;
LEDMap[3][10][1] = -1;
LEDMap[4][10][0] = -1;
LEDMap[4][10][1] = -1;
LEDMap[5][10][0] = 133;
LEDMap[5][10][1] = -1;
LEDMap[6][10][0] = -1;
LEDMap[6][10][1] = -1;
LEDMap[7][10][0] = -1;
LEDMap[7][10][1] = -1;
LEDMap[8][10][0] = -1;
LEDMap[8][10][1] = -1;
LEDMap[9][10][0] = -1;
LEDMap[9][10][1] = -1;
LEDMap[10][10][0] = 198;
LEDMap[10][10][1] = -1;
LEDMap[11][10][0] = -1;
LEDMap[11][10][1] = -1;
LEDMap[12][10][0] = -1;
LEDMap[12][10][1] = -1;
LEDMap[13][10][0] = -1;
LEDMap[13][10][1] = -1;
LEDMap[14][10][0] = -1;
LEDMap[14][10][1] = -1;
LEDMap[15][10][0] = -1;
LEDMap[15][10][1] = -1;
LEDMap[16][10][0] = -1;
LEDMap[16][10][1] = -1;
LEDMap[17][10][0] = -1;
LEDMap[17][10][1] = -1;
LEDMap[18][10][0] = -1;
LEDMap[18][10][1] = -1;
LEDMap[19][10][0] = -1;
LEDMap[19][10][1] = -1;
LEDMap[20][10][0] = -1;
LEDMap[20][10][1] = -1;
LEDMap[21][10][0] = -1;
LEDMap[21][10][1] = -1;
LEDMap[22][10][0] = -1;
LEDMap[22][10][1] = -1;
LEDMap[23][10][0] = -1;
LEDMap[23][10][1] = -1;
LEDMap[24][10][0] = -1;
LEDMap[24][10][1] = -1;
LEDMap[25][10][0] = 208;
LEDMap[25][10][1] = -1;
LEDMap[26][10][0] = 209;
LEDMap[26][10][1] = -1;
LEDMap[27][10][0] = -1;
LEDMap[27][10][1] = -1;
LEDMap[28][10][0] = 210;
LEDMap[28][10][1] = -1;
LEDMap[29][10][0] = 211;
LEDMap[29][10][1] = -1;
LEDMap[30][10][0] = -1;
LEDMap[30][10][1] = -1;
LEDMap[31][10][0] = 212;
LEDMap[31][10][1] = -1;
LEDMap[32][10][0] = 213;
LEDMap[32][10][1] = -1;
LEDMap[33][10][0] = 214;
LEDMap[33][10][1] = -1;
LEDMap[34][10][0] = -1;
LEDMap[34][10][1] = -1;
LEDMap[35][10][0] = 215;
LEDMap[35][10][1] = -1;
LEDMap[36][10][0] = -1;
LEDMap[36][10][1] = -1;
LEDMap[37][10][0] = -1;
LEDMap[37][10][1] = -1;
LEDMap[38][10][0] = -1;
LEDMap[38][10][1] = -1;
LEDMap[39][10][0] = 161;
LEDMap[39][10][1] = -1;
LEDMap[40][10][0] = -1;
LEDMap[40][10][1] = -1;
LEDMap[41][10][0] = -1;
LEDMap[41][10][1] = -1;
LEDMap[42][10][0] = 86;
LEDMap[42][10][1] = -1;
LEDMap[43][10][0] = -1;
LEDMap[43][10][1] = -1;
LEDMap[0][11][0] = -1;
LEDMap[0][11][1] = -1;
LEDMap[1][11][0] = -1;
LEDMap[1][11][1] = -1;
LEDMap[2][11][0] = -1;
LEDMap[2][11][1] = -1;
LEDMap[3][11][0] = -1;
LEDMap[3][11][1] = -1;
LEDMap[4][11][0] = -1;
LEDMap[4][11][1] = -1;
LEDMap[5][11][0] = 132;
LEDMap[5][11][1] = -1;
LEDMap[6][11][0] = -1;
LEDMap[6][11][1] = -1;
LEDMap[7][11][0] = -1;
LEDMap[7][11][1] = -1;
LEDMap[8][11][0] = -1;
LEDMap[8][11][1] = -1;
LEDMap[9][11][0] = -1;
LEDMap[9][11][1] = -1;
LEDMap[10][11][0] = 197;
LEDMap[10][11][1] = -1;
LEDMap[11][11][0] = -1;
LEDMap[11][11][1] = -1;
LEDMap[12][11][0] = -1;
LEDMap[12][11][1] = -1;
LEDMap[13][11][0] = -1;
LEDMap[13][11][1] = -1;
LEDMap[14][11][0] = -1;
LEDMap[14][11][1] = -1;
LEDMap[15][11][0] = -1;
LEDMap[15][11][1] = -1;
LEDMap[16][11][0] = -1;
LEDMap[16][11][1] = -1;
LEDMap[17][11][0] = -1;
LEDMap[17][11][1] = -1;
LEDMap[18][11][0] = -1;
LEDMap[18][11][1] = -1;
LEDMap[19][11][0] = -1;
LEDMap[19][11][1] = -1;
LEDMap[20][11][0] = -1;
LEDMap[20][11][1] = -1;
LEDMap[21][11][0] = -1;
LEDMap[21][11][1] = -1;
LEDMap[22][11][0] = -1;
LEDMap[22][11][1] = -1;
LEDMap[23][11][0] = -1;
LEDMap[23][11][1] = -1;
LEDMap[24][11][0] = -1;
LEDMap[24][11][1] = -1;
LEDMap[25][11][0] = -1;
LEDMap[25][11][1] = -1;
LEDMap[26][11][0] = -1;
LEDMap[26][11][1] = -1;
LEDMap[27][11][0] = -1;
LEDMap[27][11][1] = -1;
LEDMap[28][11][0] = -1;
LEDMap[28][11][1] = -1;
LEDMap[29][11][0] = -1;
LEDMap[29][11][1] = -1;
LEDMap[30][11][0] = -1;
LEDMap[30][11][1] = -1;
LEDMap[31][11][0] = -1;
LEDMap[31][11][1] = -1;
LEDMap[32][11][0] = -1;
LEDMap[32][11][1] = -1;
LEDMap[33][11][0] = -1;
LEDMap[33][11][1] = -1;
LEDMap[34][11][0] = -1;
LEDMap[34][11][1] = -1;
LEDMap[35][11][0] = 216;
LEDMap[35][11][1] = -1;
LEDMap[36][11][0] = -1;
LEDMap[36][11][1] = -1;
LEDMap[37][11][0] = -1;
LEDMap[37][11][1] = -1;
LEDMap[38][11][0] = -1;
LEDMap[38][11][1] = -1;
LEDMap[39][11][0] = 162;
LEDMap[39][11][1] = -1;
LEDMap[40][11][0] = -1;
LEDMap[40][11][1] = -1;
LEDMap[41][11][0] = -1;
LEDMap[41][11][1] = -1;
LEDMap[42][11][0] = -1;
LEDMap[42][11][1] = -1;
LEDMap[43][11][0] = 87;
LEDMap[43][11][1] = -1;
LEDMap[0][12][0] = 46;
LEDMap[0][12][1] = -1;
LEDMap[1][12][0] = -1;
LEDMap[1][12][1] = -1;
LEDMap[2][12][0] = -1;
LEDMap[2][12][1] = -1;
LEDMap[3][12][0] = -1;
LEDMap[3][12][1] = -1;
LEDMap[4][12][0] = -1;
LEDMap[4][12][1] = -1;
LEDMap[5][12][0] = -1;
LEDMap[5][12][1] = -1;
LEDMap[6][12][0] = -1;
LEDMap[6][12][1] = -1;
LEDMap[7][12][0] = -1;
LEDMap[7][12][1] = -1;
LEDMap[8][12][0] = -1;
LEDMap[8][12][1] = -1;
LEDMap[9][12][0] = 196;
LEDMap[9][12][1] = -1;
LEDMap[10][12][0] = -1;
LEDMap[10][12][1] = -1;
LEDMap[11][12][0] = -1;
LEDMap[11][12][1] = -1;
LEDMap[12][12][0] = -1;
LEDMap[12][12][1] = -1;
LEDMap[13][12][0] = -1;
LEDMap[13][12][1] = -1;
LEDMap[14][12][0] = -1;
LEDMap[14][12][1] = -1;
LEDMap[15][12][0] = -1;
LEDMap[15][12][1] = -1;
LEDMap[16][12][0] = -1;
LEDMap[16][12][1] = -1;
LEDMap[17][12][0] = -1;
LEDMap[17][12][1] = -1;
LEDMap[18][12][0] = -1;
LEDMap[18][12][1] = -1;
LEDMap[19][12][0] = -1;
LEDMap[19][12][1] = -1;
LEDMap[20][12][0] = -1;
LEDMap[20][12][1] = -1;
LEDMap[21][12][0] = -1;
LEDMap[21][12][1] = -1;
LEDMap[22][12][0] = -1;
LEDMap[22][12][1] = -1;
LEDMap[23][12][0] = -1;
LEDMap[23][12][1] = -1;
LEDMap[24][12][0] = -1;
LEDMap[24][12][1] = -1;
LEDMap[25][12][0] = -1;
LEDMap[25][12][1] = -1;
LEDMap[26][12][0] = -1;
LEDMap[26][12][1] = -1;
LEDMap[27][12][0] = -1;
LEDMap[27][12][1] = -1;
LEDMap[28][12][0] = -1;
LEDMap[28][12][1] = -1;
LEDMap[29][12][0] = -1;
LEDMap[29][12][1] = -1;
LEDMap[30][12][0] = -1;
LEDMap[30][12][1] = -1;
LEDMap[31][12][0] = -1;
LEDMap[31][12][1] = -1;
LEDMap[32][12][0] = -1;
LEDMap[32][12][1] = -1;
LEDMap[33][12][0] = -1;
LEDMap[33][12][1] = -1;
LEDMap[34][12][0] = -1;
LEDMap[34][12][1] = -1;
LEDMap[35][12][0] = -1;
LEDMap[35][12][1] = -1;
LEDMap[36][12][0] = 217;
LEDMap[36][12][1] = -1;
LEDMap[37][12][0] = -1;
LEDMap[37][12][1] = -1;
LEDMap[38][12][0] = -1;
LEDMap[38][12][1] = -1;
LEDMap[39][12][0] = -1;
LEDMap[39][12][1] = -1;
LEDMap[40][12][0] = -1;
LEDMap[40][12][1] = -1;
LEDMap[41][12][0] = -1;
LEDMap[41][12][1] = -1;
LEDMap[42][12][0] = -1;
LEDMap[42][12][1] = -1;
LEDMap[43][12][0] = -1;
LEDMap[43][12][1] = -1;
LEDMap[0][13][0] = 45;
LEDMap[0][13][1] = -1;
LEDMap[1][13][0] = -1;
LEDMap[1][13][1] = -1;
LEDMap[2][13][0] = -1;
LEDMap[2][13][1] = -1;
LEDMap[3][13][0] = -1;
LEDMap[3][13][1] = -1;
LEDMap[4][13][0] = -1;
LEDMap[4][13][1] = -1;
LEDMap[5][13][0] = 131;
LEDMap[5][13][1] = -1;
LEDMap[6][13][0] = -1;
LEDMap[6][13][1] = -1;
LEDMap[7][13][0] = -1;
LEDMap[7][13][1] = -1;
LEDMap[8][13][0] = -1;
LEDMap[8][13][1] = -1;
LEDMap[9][13][0] = 195;
LEDMap[9][13][1] = -1;
LEDMap[10][13][0] = -1;
LEDMap[10][13][1] = -1;
LEDMap[11][13][0] = -1;
LEDMap[11][13][1] = -1;
LEDMap[12][13][0] = -1;
LEDMap[12][13][1] = -1;
LEDMap[13][13][0] = -1;
LEDMap[13][13][1] = -1;
LEDMap[14][13][0] = -1;
LEDMap[14][13][1] = -1;
LEDMap[15][13][0] = -1;
LEDMap[15][13][1] = -1;
LEDMap[16][13][0] = -1;
LEDMap[16][13][1] = -1;
LEDMap[17][13][0] = -1;
LEDMap[17][13][1] = -1;
LEDMap[18][13][0] = -1;
LEDMap[18][13][1] = -1;
LEDMap[19][13][0] = -1;
LEDMap[19][13][1] = -1;
LEDMap[20][13][0] = -1;
LEDMap[20][13][1] = -1;
LEDMap[21][13][0] = -1;
LEDMap[21][13][1] = -1;
LEDMap[22][13][0] = -1;
LEDMap[22][13][1] = -1;
LEDMap[23][13][0] = -1;
LEDMap[23][13][1] = -1;
LEDMap[24][13][0] = -1;
LEDMap[24][13][1] = -1;
LEDMap[25][13][0] = -1;
LEDMap[25][13][1] = -1;
LEDMap[26][13][0] = -1;
LEDMap[26][13][1] = -1;
LEDMap[27][13][0] = -1;
LEDMap[27][13][1] = -1;
LEDMap[28][13][0] = -1;
LEDMap[28][13][1] = -1;
LEDMap[29][13][0] = -1;
LEDMap[29][13][1] = -1;
LEDMap[30][13][0] = -1;
LEDMap[30][13][1] = -1;
LEDMap[31][13][0] = -1;
LEDMap[31][13][1] = -1;
LEDMap[32][13][0] = -1;
LEDMap[32][13][1] = -1;
LEDMap[33][13][0] = -1;
LEDMap[33][13][1] = -1;
LEDMap[34][13][0] = -1;
LEDMap[34][13][1] = -1;
LEDMap[35][13][0] = -1;
LEDMap[35][13][1] = -1;
LEDMap[36][13][0] = -1;
LEDMap[36][13][1] = -1;
LEDMap[37][13][0] = -1;
LEDMap[37][13][1] = -1;
LEDMap[38][13][0] = -1;
LEDMap[38][13][1] = -1;
LEDMap[39][13][0] = 163;
LEDMap[39][13][1] = -1;
LEDMap[40][13][0] = -1;
LEDMap[40][13][1] = -1;
LEDMap[41][13][0] = -1;
LEDMap[41][13][1] = -1;
LEDMap[42][13][0] = 88;
LEDMap[42][13][1] = -1;
LEDMap[43][13][0] = -1;
LEDMap[43][13][1] = -1;
LEDMap[0][14][0] = -1;
LEDMap[0][14][1] = -1;
LEDMap[1][14][0] = -1;
LEDMap[1][14][1] = -1;
LEDMap[2][14][0] = -1;
LEDMap[2][14][1] = -1;
LEDMap[3][14][0] = -1;
LEDMap[3][14][1] = -1;
LEDMap[4][14][0] = -1;
LEDMap[4][14][1] = -1;
LEDMap[5][14][0] = 130;
LEDMap[5][14][1] = -1;
LEDMap[6][14][0] = -1;
LEDMap[6][14][1] = -1;
LEDMap[7][14][0] = -1;
LEDMap[7][14][1] = -1;
LEDMap[8][14][0] = -1;
LEDMap[8][14][1] = -1;
LEDMap[9][14][0] = -1;
LEDMap[9][14][1] = -1;
LEDMap[10][14][0] = -1;
LEDMap[10][14][1] = -1;
LEDMap[11][14][0] = -1;
LEDMap[11][14][1] = -1;
LEDMap[12][14][0] = -1;
LEDMap[12][14][1] = -1;
LEDMap[13][14][0] = -1;
LEDMap[13][14][1] = -1;
LEDMap[14][14][0] = -1;
LEDMap[14][14][1] = -1;
LEDMap[15][14][0] = -1;
LEDMap[15][14][1] = -1;
LEDMap[16][14][0] = -1;
LEDMap[16][14][1] = -1;
LEDMap[17][14][0] = -1;
LEDMap[17][14][1] = -1;
LEDMap[18][14][0] = -1;
LEDMap[18][14][1] = -1;
LEDMap[19][14][0] = -1;
LEDMap[19][14][1] = -1;
LEDMap[20][14][0] = -1;
LEDMap[20][14][1] = -1;
LEDMap[21][14][0] = -1;
LEDMap[21][14][1] = -1;
LEDMap[22][14][0] = -1;
LEDMap[22][14][1] = -1;
LEDMap[23][14][0] = -1;
LEDMap[23][14][1] = -1;
LEDMap[24][14][0] = -1;
LEDMap[24][14][1] = -1;
LEDMap[25][14][0] = -1;
LEDMap[25][14][1] = -1;
LEDMap[26][14][0] = -1;
LEDMap[26][14][1] = -1;
LEDMap[27][14][0] = -1;
LEDMap[27][14][1] = -1;
LEDMap[28][14][0] = -1;
LEDMap[28][14][1] = -1;
LEDMap[29][14][0] = -1;
LEDMap[29][14][1] = -1;
LEDMap[30][14][0] = -1;
LEDMap[30][14][1] = -1;
LEDMap[31][14][0] = -1;
LEDMap[31][14][1] = -1;
LEDMap[32][14][0] = -1;
LEDMap[32][14][1] = -1;
LEDMap[33][14][0] = -1;
LEDMap[33][14][1] = -1;
LEDMap[34][14][0] = -1;
LEDMap[34][14][1] = -1;
LEDMap[35][14][0] = 218;
LEDMap[35][14][1] = -1;
LEDMap[36][14][0] = -1;
LEDMap[36][14][1] = -1;
LEDMap[37][14][0] = -1;
LEDMap[37][14][1] = -1;
LEDMap[38][14][0] = -1;
LEDMap[38][14][1] = -1;
LEDMap[39][14][0] = 164;
LEDMap[39][14][1] = -1;
LEDMap[40][14][0] = -1;
LEDMap[40][14][1] = -1;
LEDMap[41][14][0] = -1;
LEDMap[41][14][1] = -1;
LEDMap[42][14][0] = -1;
LEDMap[42][14][1] = -1;
LEDMap[43][14][0] = 89;
LEDMap[43][14][1] = -1;
LEDMap[0][15][0] = 44;
LEDMap[0][15][1] = -1;
LEDMap[1][15][0] = -1;
LEDMap[1][15][1] = -1;
LEDMap[2][15][0] = -1;
LEDMap[2][15][1] = -1;
LEDMap[3][15][0] = -1;
LEDMap[3][15][1] = -1;
LEDMap[4][15][0] = -1;
LEDMap[4][15][1] = -1;
LEDMap[5][15][0] = -1;
LEDMap[5][15][1] = -1;
LEDMap[6][15][0] = -1;
LEDMap[6][15][1] = -1;
LEDMap[7][15][0] = -1;
LEDMap[7][15][1] = -1;
LEDMap[8][15][0] = -1;
LEDMap[8][15][1] = -1;
LEDMap[9][15][0] = 194;
LEDMap[9][15][1] = -1;
LEDMap[10][15][0] = -1;
LEDMap[10][15][1] = -1;
LEDMap[11][15][0] = -1;
LEDMap[11][15][1] = -1;
LEDMap[12][15][0] = -1;
LEDMap[12][15][1] = -1;
LEDMap[13][15][0] = -1;
LEDMap[13][15][1] = -1;
LEDMap[14][15][0] = -1;
LEDMap[14][15][1] = -1;
LEDMap[15][15][0] = -1;
LEDMap[15][15][1] = -1;
LEDMap[16][15][0] = -1;
LEDMap[16][15][1] = -1;
LEDMap[17][15][0] = -1;
LEDMap[17][15][1] = -1;
LEDMap[18][15][0] = -1;
LEDMap[18][15][1] = -1;
LEDMap[19][15][0] = -1;
LEDMap[19][15][1] = -1;
LEDMap[20][15][0] = -1;
LEDMap[20][15][1] = -1;
LEDMap[21][15][0] = -1;
LEDMap[21][15][1] = -1;
LEDMap[22][15][0] = -1;
LEDMap[22][15][1] = -1;
LEDMap[23][15][0] = -1;
LEDMap[23][15][1] = -1;
LEDMap[24][15][0] = -1;
LEDMap[24][15][1] = -1;
LEDMap[25][15][0] = -1;
LEDMap[25][15][1] = -1;
LEDMap[26][15][0] = -1;
LEDMap[26][15][1] = -1;
LEDMap[27][15][0] = -1;
LEDMap[27][15][1] = -1;
LEDMap[28][15][0] = -1;
LEDMap[28][15][1] = -1;
LEDMap[29][15][0] = -1;
LEDMap[29][15][1] = -1;
LEDMap[30][15][0] = -1;
LEDMap[30][15][1] = -1;
LEDMap[31][15][0] = -1;
LEDMap[31][15][1] = -1;
LEDMap[32][15][0] = -1;
LEDMap[32][15][1] = -1;
LEDMap[33][15][0] = -1;
LEDMap[33][15][1] = -1;
LEDMap[34][15][0] = -1;
LEDMap[34][15][1] = -1;
LEDMap[35][15][0] = 219;
LEDMap[35][15][1] = -1;
LEDMap[36][15][0] = -1;
LEDMap[36][15][1] = -1;
LEDMap[37][15][0] = -1;
LEDMap[37][15][1] = -1;
LEDMap[38][15][0] = -1;
LEDMap[38][15][1] = -1;
LEDMap[39][15][0] = -1;
LEDMap[39][15][1] = -1;
LEDMap[40][15][0] = -1;
LEDMap[40][15][1] = -1;
LEDMap[41][15][0] = -1;
LEDMap[41][15][1] = -1;
LEDMap[42][15][0] = -1;
LEDMap[42][15][1] = -1;
LEDMap[43][15][0] = -1;
LEDMap[43][15][1] = -1;
LEDMap[0][16][0] = 43;
LEDMap[0][16][1] = -1;
LEDMap[1][16][0] = -1;
LEDMap[1][16][1] = -1;
LEDMap[2][16][0] = -1;
LEDMap[2][16][1] = -1;
LEDMap[3][16][0] = -1;
LEDMap[3][16][1] = -1;
LEDMap[4][16][0] = -1;
LEDMap[4][16][1] = -1;
LEDMap[5][16][0] = 129;
LEDMap[5][16][1] = -1;
LEDMap[6][16][0] = -1;
LEDMap[6][16][1] = -1;
LEDMap[7][16][0] = -1;
LEDMap[7][16][1] = -1;
LEDMap[8][16][0] = -1;
LEDMap[8][16][1] = -1;
LEDMap[9][16][0] = 193;
LEDMap[9][16][1] = -1;
LEDMap[10][16][0] = -1;
LEDMap[10][16][1] = -1;
LEDMap[11][16][0] = -1;
LEDMap[11][16][1] = -1;
LEDMap[12][16][0] = -1;
LEDMap[12][16][1] = -1;
LEDMap[13][16][0] = -1;
LEDMap[13][16][1] = -1;
LEDMap[14][16][0] = 235;
LEDMap[14][16][1] = -1;
LEDMap[15][16][0] = 234;
LEDMap[15][16][1] = -1;
LEDMap[16][16][0] = 233;
LEDMap[16][16][1] = -1;
LEDMap[17][16][0] = -1;
LEDMap[17][16][1] = -1;
LEDMap[18][16][0] = 232;
LEDMap[18][16][1] = -1;
LEDMap[19][16][0] = 231;
LEDMap[19][16][1] = -1;
LEDMap[20][16][0] = -1;
LEDMap[20][16][1] = -1;
LEDMap[21][16][0] = 230;
LEDMap[21][16][1] = -1;
LEDMap[22][16][0] = 229;
LEDMap[22][16][1] = -1;
LEDMap[23][16][0] = -1;
LEDMap[23][16][1] = -1;
LEDMap[24][16][0] = 228;
LEDMap[24][16][1] = -1;
LEDMap[25][16][0] = 227;
LEDMap[25][16][1] = -1;
LEDMap[26][16][0] = 226;
LEDMap[26][16][1] = -1;
LEDMap[27][16][0] = -1;
LEDMap[27][16][1] = -1;
LEDMap[28][16][0] = 225;
LEDMap[28][16][1] = -1;
LEDMap[29][16][0] = 224;
LEDMap[29][16][1] = -1;
LEDMap[30][16][0] = -1;
LEDMap[30][16][1] = -1;
LEDMap[31][16][0] = 223;
LEDMap[31][16][1] = -1;
LEDMap[32][16][0] = 222;
LEDMap[32][16][1] = -1;
LEDMap[33][16][0] = -1;
LEDMap[33][16][1] = -1;
LEDMap[34][16][0] = 220;
LEDMap[34][16][1] = 221;
LEDMap[35][16][0] = -1;
LEDMap[35][16][1] = -1;
LEDMap[36][16][0] = -1;
LEDMap[36][16][1] = -1;
LEDMap[37][16][0] = -1;
LEDMap[37][16][1] = -1;
LEDMap[38][16][0] = -1;
LEDMap[38][16][1] = -1;
LEDMap[39][16][0] = 165;
LEDMap[39][16][1] = -1;
LEDMap[40][16][0] = -1;
LEDMap[40][16][1] = -1;
LEDMap[41][16][0] = -1;
LEDMap[41][16][1] = -1;
LEDMap[42][16][0] = -1;
LEDMap[42][16][1] = -1;
LEDMap[43][16][0] = 90;
LEDMap[43][16][1] = -1;
LEDMap[0][17][0] = -1;
LEDMap[0][17][1] = -1;
LEDMap[1][17][0] = -1;
LEDMap[1][17][1] = -1;
LEDMap[2][17][0] = -1;
LEDMap[2][17][1] = -1;
LEDMap[3][17][0] = -1;
LEDMap[3][17][1] = -1;
LEDMap[4][17][0] = -1;
LEDMap[4][17][1] = -1;
LEDMap[5][17][0] = 128;
LEDMap[5][17][1] = -1;
LEDMap[6][17][0] = -1;
LEDMap[6][17][1] = -1;
LEDMap[7][17][0] = -1;
LEDMap[7][17][1] = -1;
LEDMap[8][17][0] = -1;
LEDMap[8][17][1] = -1;
LEDMap[9][17][0] = -1;
LEDMap[9][17][1] = -1;
LEDMap[10][17][0] = -1;
LEDMap[10][17][1] = -1;
LEDMap[11][17][0] = -1;
LEDMap[11][17][1] = -1;
LEDMap[12][17][0] = -1;
LEDMap[12][17][1] = -1;
LEDMap[13][17][0] = -1;
LEDMap[13][17][1] = -1;
LEDMap[14][17][0] = -1;
LEDMap[14][17][1] = -1;
LEDMap[15][17][0] = -1;
LEDMap[15][17][1] = -1;
LEDMap[16][17][0] = -1;
LEDMap[16][17][1] = -1;
LEDMap[17][17][0] = -1;
LEDMap[17][17][1] = -1;
LEDMap[18][17][0] = -1;
LEDMap[18][17][1] = -1;
LEDMap[19][17][0] = -1;
LEDMap[19][17][1] = -1;
LEDMap[20][17][0] = -1;
LEDMap[20][17][1] = -1;
LEDMap[21][17][0] = -1;
LEDMap[21][17][1] = -1;
LEDMap[22][17][0] = -1;
LEDMap[22][17][1] = -1;
LEDMap[23][17][0] = -1;
LEDMap[23][17][1] = -1;
LEDMap[24][17][0] = -1;
LEDMap[24][17][1] = -1;
LEDMap[25][17][0] = -1;
LEDMap[25][17][1] = -1;
LEDMap[26][17][0] = -1;
LEDMap[26][17][1] = -1;
LEDMap[27][17][0] = -1;
LEDMap[27][17][1] = -1;
LEDMap[28][17][0] = -1;
LEDMap[28][17][1] = -1;
LEDMap[29][17][0] = -1;
LEDMap[29][17][1] = -1;
LEDMap[30][17][0] = -1;
LEDMap[30][17][1] = -1;
LEDMap[31][17][0] = -1;
LEDMap[31][17][1] = -1;
LEDMap[32][17][0] = -1;
LEDMap[32][17][1] = -1;
LEDMap[33][17][0] = -1;
LEDMap[33][17][1] = -1;
LEDMap[34][17][0] = -1;
LEDMap[34][17][1] = -1;
LEDMap[35][17][0] = -1;
LEDMap[35][17][1] = -1;
LEDMap[36][17][0] = -1;
LEDMap[36][17][1] = -1;
LEDMap[37][17][0] = -1;
LEDMap[37][17][1] = -1;
LEDMap[38][17][0] = -1;
LEDMap[38][17][1] = -1;
LEDMap[39][17][0] = 166;
LEDMap[39][17][1] = -1;
LEDMap[40][17][0] = -1;
LEDMap[40][17][1] = -1;
LEDMap[41][17][0] = -1;
LEDMap[41][17][1] = -1;
LEDMap[42][17][0] = -1;
LEDMap[42][17][1] = -1;
LEDMap[43][17][0] = 91;
LEDMap[43][17][1] = -1;
LEDMap[0][18][0] = 42;
LEDMap[0][18][1] = -1;
LEDMap[1][18][0] = -1;
LEDMap[1][18][1] = -1;
LEDMap[2][18][0] = -1;
LEDMap[2][18][1] = -1;
LEDMap[3][18][0] = -1;
LEDMap[3][18][1] = -1;
LEDMap[4][18][0] = -1;
LEDMap[4][18][1] = -1;
LEDMap[5][18][0] = -1;
LEDMap[5][18][1] = -1;
LEDMap[6][18][0] = -1;
LEDMap[6][18][1] = -1;
LEDMap[7][18][0] = -1;
LEDMap[7][18][1] = -1;
LEDMap[8][18][0] = -1;
LEDMap[8][18][1] = -1;
LEDMap[9][18][0] = 192;
LEDMap[9][18][1] = -1;
LEDMap[10][18][0] = -1;
LEDMap[10][18][1] = -1;
LEDMap[11][18][0] = -1;
LEDMap[11][18][1] = -1;
LEDMap[12][18][0] = -1;
LEDMap[12][18][1] = -1;
LEDMap[13][18][0] = -1;
LEDMap[13][18][1] = -1;
LEDMap[14][18][0] = -1;
LEDMap[14][18][1] = -1;
LEDMap[15][18][0] = -1;
LEDMap[15][18][1] = -1;
LEDMap[16][18][0] = -1;
LEDMap[16][18][1] = -1;
LEDMap[17][18][0] = -1;
LEDMap[17][18][1] = -1;
LEDMap[18][18][0] = -1;
LEDMap[18][18][1] = -1;
LEDMap[19][18][0] = -1;
LEDMap[19][18][1] = -1;
LEDMap[20][18][0] = -1;
LEDMap[20][18][1] = -1;
LEDMap[21][18][0] = -1;
LEDMap[21][18][1] = -1;
LEDMap[22][18][0] = -1;
LEDMap[22][18][1] = -1;
LEDMap[23][18][0] = -1;
LEDMap[23][18][1] = -1;
LEDMap[24][18][0] = -1;
LEDMap[24][18][1] = -1;
LEDMap[25][18][0] = -1;
LEDMap[25][18][1] = -1;
LEDMap[26][18][0] = -1;
LEDMap[26][18][1] = -1;
LEDMap[27][18][0] = -1;
LEDMap[27][18][1] = -1;
LEDMap[28][18][0] = -1;
LEDMap[28][18][1] = -1;
LEDMap[29][18][0] = -1;
LEDMap[29][18][1] = -1;
LEDMap[30][18][0] = -1;
LEDMap[30][18][1] = -1;
LEDMap[31][18][0] = -1;
LEDMap[31][18][1] = -1;
LEDMap[32][18][0] = -1;
LEDMap[32][18][1] = -1;
LEDMap[33][18][0] = -1;
LEDMap[33][18][1] = -1;
LEDMap[34][18][0] = -1;
LEDMap[34][18][1] = -1;
LEDMap[35][18][0] = -1;
LEDMap[35][18][1] = -1;
LEDMap[36][18][0] = -1;
LEDMap[36][18][1] = -1;
LEDMap[37][18][0] = -1;
LEDMap[37][18][1] = -1;
LEDMap[38][18][0] = -1;
LEDMap[38][18][1] = -1;
LEDMap[39][18][0] = 167;
LEDMap[39][18][1] = -1;
LEDMap[40][18][0] = -1;
LEDMap[40][18][1] = -1;
LEDMap[41][18][0] = -1;
LEDMap[41][18][1] = -1;
LEDMap[42][18][0] = 92;
LEDMap[42][18][1] = -1;
LEDMap[43][18][0] = -1;
LEDMap[43][18][1] = -1;
LEDMap[0][19][0] = 41;
LEDMap[0][19][1] = -1;
LEDMap[1][19][0] = -1;
LEDMap[1][19][1] = -1;
LEDMap[2][19][0] = -1;
LEDMap[2][19][1] = -1;
LEDMap[3][19][0] = -1;
LEDMap[3][19][1] = -1;
LEDMap[4][19][0] = -1;
LEDMap[4][19][1] = -1;
LEDMap[5][19][0] = 127;
LEDMap[5][19][1] = -1;
LEDMap[6][19][0] = -1;
LEDMap[6][19][1] = -1;
LEDMap[7][19][0] = -1;
LEDMap[7][19][1] = -1;
LEDMap[8][19][0] = -1;
LEDMap[8][19][1] = -1;
LEDMap[9][19][0] = -1;
LEDMap[9][19][1] = -1;
LEDMap[10][19][0] = 191;
LEDMap[10][19][1] = -1;
LEDMap[11][19][0] = -1;
LEDMap[11][19][1] = -1;
LEDMap[12][19][0] = -1;
LEDMap[12][19][1] = -1;
LEDMap[13][19][0] = -1;
LEDMap[13][19][1] = -1;
LEDMap[14][19][0] = -1;
LEDMap[14][19][1] = -1;
LEDMap[15][19][0] = -1;
LEDMap[15][19][1] = -1;
LEDMap[16][19][0] = -1;
LEDMap[16][19][1] = -1;
LEDMap[17][19][0] = -1;
LEDMap[17][19][1] = -1;
LEDMap[18][19][0] = -1;
LEDMap[18][19][1] = -1;
LEDMap[19][19][0] = -1;
LEDMap[19][19][1] = -1;
LEDMap[20][19][0] = -1;
LEDMap[20][19][1] = -1;
LEDMap[21][19][0] = -1;
LEDMap[21][19][1] = -1;
LEDMap[22][19][0] = -1;
LEDMap[22][19][1] = -1;
LEDMap[23][19][0] = -1;
LEDMap[23][19][1] = -1;
LEDMap[24][19][0] = -1;
LEDMap[24][19][1] = -1;
LEDMap[25][19][0] = -1;
LEDMap[25][19][1] = -1;
LEDMap[26][19][0] = -1;
LEDMap[26][19][1] = -1;
LEDMap[27][19][0] = -1;
LEDMap[27][19][1] = -1;
LEDMap[28][19][0] = -1;
LEDMap[28][19][1] = -1;
LEDMap[29][19][0] = -1;
LEDMap[29][19][1] = -1;
LEDMap[30][19][0] = -1;
LEDMap[30][19][1] = -1;
LEDMap[31][19][0] = -1;
LEDMap[31][19][1] = -1;
LEDMap[32][19][0] = -1;
LEDMap[32][19][1] = -1;
LEDMap[33][19][0] = -1;
LEDMap[33][19][1] = -1;
LEDMap[34][19][0] = -1;
LEDMap[34][19][1] = -1;
LEDMap[35][19][0] = -1;
LEDMap[35][19][1] = -1;
LEDMap[36][19][0] = -1;
LEDMap[36][19][1] = -1;
LEDMap[37][19][0] = -1;
LEDMap[37][19][1] = -1;
LEDMap[38][19][0] = -1;
LEDMap[38][19][1] = -1;
LEDMap[39][19][0] = -1;
LEDMap[39][19][1] = -1;
LEDMap[40][19][0] = -1;
LEDMap[40][19][1] = -1;
LEDMap[41][19][0] = -1;
LEDMap[41][19][1] = -1;
LEDMap[42][19][0] = -1;
LEDMap[42][19][1] = -1;
LEDMap[43][19][0] = -1;
LEDMap[43][19][1] = -1;
LEDMap[0][20][0] = -1;
LEDMap[0][20][1] = -1;
LEDMap[1][20][0] = -1;
LEDMap[1][20][1] = -1;
LEDMap[2][20][0] = -1;
LEDMap[2][20][1] = -1;
LEDMap[3][20][0] = -1;
LEDMap[3][20][1] = -1;
LEDMap[4][20][0] = -1;
LEDMap[4][20][1] = -1;
LEDMap[5][20][0] = 126;
LEDMap[5][20][1] = -1;
LEDMap[6][20][0] = -1;
LEDMap[6][20][1] = -1;
LEDMap[7][20][0] = -1;
LEDMap[7][20][1] = -1;
LEDMap[8][20][0] = -1;
LEDMap[8][20][1] = -1;
LEDMap[9][20][0] = -1;
LEDMap[9][20][1] = -1;
LEDMap[10][20][0] = 190;
LEDMap[10][20][1] = -1;
LEDMap[11][20][0] = 189;
LEDMap[11][20][1] = -1;
LEDMap[12][20][0] = -1;
LEDMap[12][20][1] = -1;
LEDMap[13][20][0] = -1;
LEDMap[13][20][1] = -1;
LEDMap[14][20][0] = -1;
LEDMap[14][20][1] = -1;
LEDMap[15][20][0] = -1;
LEDMap[15][20][1] = -1;
LEDMap[16][20][0] = -1;
LEDMap[16][20][1] = -1;
LEDMap[17][20][0] = -1;
LEDMap[17][20][1] = -1;
LEDMap[18][20][0] = -1;
LEDMap[18][20][1] = -1;
LEDMap[19][20][0] = -1;
LEDMap[19][20][1] = -1;
LEDMap[20][20][0] = -1;
LEDMap[20][20][1] = -1;
LEDMap[21][20][0] = -1;
LEDMap[21][20][1] = -1;
LEDMap[22][20][0] = -1;
LEDMap[22][20][1] = -1;
LEDMap[23][20][0] = -1;
LEDMap[23][20][1] = -1;
LEDMap[24][20][0] = -1;
LEDMap[24][20][1] = -1;
LEDMap[25][20][0] = -1;
LEDMap[25][20][1] = -1;
LEDMap[26][20][0] = -1;
LEDMap[26][20][1] = -1;
LEDMap[27][20][0] = -1;
LEDMap[27][20][1] = -1;
LEDMap[28][20][0] = -1;
LEDMap[28][20][1] = -1;
LEDMap[29][20][0] = -1;
LEDMap[29][20][1] = -1;
LEDMap[30][20][0] = -1;
LEDMap[30][20][1] = -1;
LEDMap[31][20][0] = -1;
LEDMap[31][20][1] = -1;
LEDMap[32][20][0] = -1;
LEDMap[32][20][1] = -1;
LEDMap[33][20][0] = -1;
LEDMap[33][20][1] = -1;
LEDMap[34][20][0] = -1;
LEDMap[34][20][1] = -1;
LEDMap[35][20][0] = -1;
LEDMap[35][20][1] = -1;
LEDMap[36][20][0] = -1;
LEDMap[36][20][1] = -1;
LEDMap[37][20][0] = -1;
LEDMap[37][20][1] = -1;
LEDMap[38][20][0] = -1;
LEDMap[38][20][1] = -1;
LEDMap[39][20][0] = 168;
LEDMap[39][20][1] = -1;
LEDMap[40][20][0] = -1;
LEDMap[40][20][1] = -1;
LEDMap[41][20][0] = -1;
LEDMap[41][20][1] = -1;
LEDMap[42][20][0] = 93;
LEDMap[42][20][1] = -1;
LEDMap[43][20][0] = -1;
LEDMap[43][20][1] = -1;
LEDMap[0][21][0] = 40;
LEDMap[0][21][1] = -1;
LEDMap[1][21][0] = -1;
LEDMap[1][21][1] = -1;
LEDMap[2][21][0] = -1;
LEDMap[2][21][1] = -1;
LEDMap[3][21][0] = -1;
LEDMap[3][21][1] = -1;
LEDMap[4][21][0] = -1;
LEDMap[4][21][1] = -1;
LEDMap[5][21][0] = -1;
LEDMap[5][21][1] = -1;
LEDMap[6][21][0] = -1;
LEDMap[6][21][1] = -1;
LEDMap[7][21][0] = -1;
LEDMap[7][21][1] = -1;
LEDMap[8][21][0] = -1;
LEDMap[8][21][1] = -1;
LEDMap[9][21][0] = -1;
LEDMap[9][21][1] = -1;
LEDMap[10][21][0] = -1;
LEDMap[10][21][1] = -1;
LEDMap[11][21][0] = -1;
LEDMap[11][21][1] = -1;
LEDMap[12][21][0] = -1;
LEDMap[12][21][1] = -1;
LEDMap[13][21][0] = 188;
LEDMap[13][21][1] = -1;
LEDMap[14][21][0] = 187;
LEDMap[14][21][1] = -1;
LEDMap[15][21][0] = 186;
LEDMap[15][21][1] = -1;
LEDMap[16][21][0] = -1;
LEDMap[16][21][1] = -1;
LEDMap[17][21][0] = 185;
LEDMap[17][21][1] = -1;
LEDMap[18][21][0] = -1;
LEDMap[18][21][1] = -1;
LEDMap[19][21][0] = 184;
LEDMap[19][21][1] = -1;
LEDMap[20][21][0] = 183;
LEDMap[20][21][1] = -1;
LEDMap[21][21][0] = 182;
LEDMap[21][21][1] = -1;
LEDMap[22][21][0] = -1;
LEDMap[22][21][1] = -1;
LEDMap[23][21][0] = 181;
LEDMap[23][21][1] = -1;
LEDMap[24][21][0] = 180;
LEDMap[24][21][1] = -1;
LEDMap[25][21][0] = -1;
LEDMap[25][21][1] = -1;
LEDMap[26][21][0] = 179;
LEDMap[26][21][1] = -1;
LEDMap[27][21][0] = -1;
LEDMap[27][21][1] = -1;
LEDMap[28][21][0] = -1;
LEDMap[28][21][1] = -1;
LEDMap[29][21][0] = -1;
LEDMap[29][21][1] = -1;
LEDMap[30][21][0] = -1;
LEDMap[30][21][1] = -1;
LEDMap[31][21][0] = -1;
LEDMap[31][21][1] = -1;
LEDMap[32][21][0] = -1;
LEDMap[32][21][1] = -1;
LEDMap[33][21][0] = -1;
LEDMap[33][21][1] = -1;
LEDMap[34][21][0] = -1;
LEDMap[34][21][1] = -1;
LEDMap[35][21][0] = -1;
LEDMap[35][21][1] = -1;
LEDMap[36][21][0] = -1;
LEDMap[36][21][1] = -1;
LEDMap[37][21][0] = -1;
LEDMap[37][21][1] = -1;
LEDMap[38][21][0] = 169;
LEDMap[38][21][1] = -1;
LEDMap[39][21][0] = -1;
LEDMap[39][21][1] = -1;
LEDMap[40][21][0] = -1;
LEDMap[40][21][1] = -1;
LEDMap[41][21][0] = -1;
LEDMap[41][21][1] = -1;
LEDMap[42][21][0] = 94;
LEDMap[42][21][1] = -1;
LEDMap[43][21][0] = -1;
LEDMap[43][21][1] = -1;
LEDMap[0][22][0] = 39;
LEDMap[0][22][1] = -1;
LEDMap[1][22][0] = -1;
LEDMap[1][22][1] = -1;
LEDMap[2][22][0] = -1;
LEDMap[2][22][1] = -1;
LEDMap[3][22][0] = -1;
LEDMap[3][22][1] = -1;
LEDMap[4][22][0] = -1;
LEDMap[4][22][1] = -1;
LEDMap[5][22][0] = 125;
LEDMap[5][22][1] = -1;
LEDMap[6][22][0] = -1;
LEDMap[6][22][1] = -1;
LEDMap[7][22][0] = -1;
LEDMap[7][22][1] = -1;
LEDMap[8][22][0] = -1;
LEDMap[8][22][1] = -1;
LEDMap[9][22][0] = -1;
LEDMap[9][22][1] = -1;
LEDMap[10][22][0] = -1;
LEDMap[10][22][1] = -1;
LEDMap[11][22][0] = -1;
LEDMap[11][22][1] = -1;
LEDMap[12][22][0] = -1;
LEDMap[12][22][1] = -1;
LEDMap[13][22][0] = -1;
LEDMap[13][22][1] = -1;
LEDMap[14][22][0] = -1;
LEDMap[14][22][1] = -1;
LEDMap[15][22][0] = -1;
LEDMap[15][22][1] = -1;
LEDMap[16][22][0] = -1;
LEDMap[16][22][1] = -1;
LEDMap[17][22][0] = -1;
LEDMap[17][22][1] = -1;
LEDMap[18][22][0] = -1;
LEDMap[18][22][1] = -1;
LEDMap[19][22][0] = -1;
LEDMap[19][22][1] = -1;
LEDMap[20][22][0] = -1;
LEDMap[20][22][1] = -1;
LEDMap[21][22][0] = -1;
LEDMap[21][22][1] = -1;
LEDMap[22][22][0] = -1;
LEDMap[22][22][1] = -1;
LEDMap[23][22][0] = -1;
LEDMap[23][22][1] = -1;
LEDMap[24][22][0] = -1;
LEDMap[24][22][1] = -1;
LEDMap[25][22][0] = -1;
LEDMap[25][22][1] = -1;
LEDMap[26][22][0] = -1;
LEDMap[26][22][1] = -1;
LEDMap[27][22][0] = 178;
LEDMap[27][22][1] = -1;
LEDMap[28][22][0] = 177;
LEDMap[28][22][1] = -1;
LEDMap[29][22][0] = -1;
LEDMap[29][22][1] = -1;
LEDMap[30][22][0] = 176;
LEDMap[30][22][1] = -1;
LEDMap[31][22][0] = 175;
LEDMap[31][22][1] = -1;
LEDMap[32][22][0] = -1;
LEDMap[32][22][1] = -1;
LEDMap[33][22][0] = 174;
LEDMap[33][22][1] = -1;
LEDMap[34][22][0] = 173;
LEDMap[34][22][1] = -1;
LEDMap[35][22][0] = 172;
LEDMap[35][22][1] = -1;
LEDMap[36][22][0] = -1;
LEDMap[36][22][1] = -1;
LEDMap[37][22][0] = 171;
LEDMap[37][22][1] = -1;
LEDMap[38][22][0] = 170;
LEDMap[38][22][1] = -1;
LEDMap[39][22][0] = -1;
LEDMap[39][22][1] = -1;
LEDMap[40][22][0] = -1;
LEDMap[40][22][1] = -1;
LEDMap[41][22][0] = 95;
LEDMap[41][22][1] = -1;
LEDMap[42][22][0] = -1;
LEDMap[42][22][1] = -1;
LEDMap[43][22][0] = -1;
LEDMap[43][22][1] = -1;
LEDMap[0][23][0] = -1;
LEDMap[0][23][1] = -1;
LEDMap[1][23][0] = -1;
LEDMap[1][23][1] = -1;
LEDMap[2][23][0] = -1;
LEDMap[2][23][1] = -1;
LEDMap[3][23][0] = -1;
LEDMap[3][23][1] = -1;
LEDMap[4][23][0] = -1;
LEDMap[4][23][1] = -1;
LEDMap[5][23][0] = 124;
LEDMap[5][23][1] = -1;
LEDMap[6][23][0] = -1;
LEDMap[6][23][1] = -1;
LEDMap[7][23][0] = -1;
LEDMap[7][23][1] = -1;
LEDMap[8][23][0] = -1;
LEDMap[8][23][1] = -1;
LEDMap[9][23][0] = -1;
LEDMap[9][23][1] = -1;
LEDMap[10][23][0] = -1;
LEDMap[10][23][1] = -1;
LEDMap[11][23][0] = -1;
LEDMap[11][23][1] = -1;
LEDMap[12][23][0] = -1;
LEDMap[12][23][1] = -1;
LEDMap[13][23][0] = -1;
LEDMap[13][23][1] = -1;
LEDMap[14][23][0] = -1;
LEDMap[14][23][1] = -1;
LEDMap[15][23][0] = -1;
LEDMap[15][23][1] = -1;
LEDMap[16][23][0] = -1;
LEDMap[16][23][1] = -1;
LEDMap[17][23][0] = -1;
LEDMap[17][23][1] = -1;
LEDMap[18][23][0] = -1;
LEDMap[18][23][1] = -1;
LEDMap[19][23][0] = -1;
LEDMap[19][23][1] = -1;
LEDMap[20][23][0] = -1;
LEDMap[20][23][1] = -1;
LEDMap[21][23][0] = -1;
LEDMap[21][23][1] = -1;
LEDMap[22][23][0] = -1;
LEDMap[22][23][1] = -1;
LEDMap[23][23][0] = -1;
LEDMap[23][23][1] = -1;
LEDMap[24][23][0] = -1;
LEDMap[24][23][1] = -1;
LEDMap[25][23][0] = -1;
LEDMap[25][23][1] = -1;
LEDMap[26][23][0] = -1;
LEDMap[26][23][1] = -1;
LEDMap[27][23][0] = -1;
LEDMap[27][23][1] = -1;
LEDMap[28][23][0] = -1;
LEDMap[28][23][1] = -1;
LEDMap[29][23][0] = -1;
LEDMap[29][23][1] = -1;
LEDMap[30][23][0] = -1;
LEDMap[30][23][1] = -1;
LEDMap[31][23][0] = -1;
LEDMap[31][23][1] = -1;
LEDMap[32][23][0] = -1;
LEDMap[32][23][1] = -1;
LEDMap[33][23][0] = -1;
LEDMap[33][23][1] = -1;
LEDMap[34][23][0] = -1;
LEDMap[34][23][1] = -1;
LEDMap[35][23][0] = -1;
LEDMap[35][23][1] = -1;
LEDMap[36][23][0] = -1;
LEDMap[36][23][1] = -1;
LEDMap[37][23][0] = -1;
LEDMap[37][23][1] = -1;
LEDMap[38][23][0] = -1;
LEDMap[38][23][1] = -1;
LEDMap[39][23][0] = -1;
LEDMap[39][23][1] = -1;
LEDMap[40][23][0] = -1;
LEDMap[40][23][1] = -1;
LEDMap[41][23][0] = -1;
LEDMap[41][23][1] = -1;
LEDMap[42][23][0] = -1;
LEDMap[42][23][1] = -1;
LEDMap[43][23][0] = -1;
LEDMap[43][23][1] = -1;
LEDMap[0][24][0] = 38;
LEDMap[0][24][1] = -1;
LEDMap[1][24][0] = -1;
LEDMap[1][24][1] = -1;
LEDMap[2][24][0] = -1;
LEDMap[2][24][1] = -1;
LEDMap[3][24][0] = -1;
LEDMap[3][24][1] = -1;
LEDMap[4][24][0] = -1;
LEDMap[4][24][1] = -1;
LEDMap[5][24][0] = 123;
LEDMap[5][24][1] = -1;
LEDMap[6][24][0] = -1;
LEDMap[6][24][1] = -1;
LEDMap[7][24][0] = -1;
LEDMap[7][24][1] = -1;
LEDMap[8][24][0] = -1;
LEDMap[8][24][1] = -1;
LEDMap[9][24][0] = -1;
LEDMap[9][24][1] = -1;
LEDMap[10][24][0] = -1;
LEDMap[10][24][1] = -1;
LEDMap[11][24][0] = -1;
LEDMap[11][24][1] = -1;
LEDMap[12][24][0] = -1;
LEDMap[12][24][1] = -1;
LEDMap[13][24][0] = -1;
LEDMap[13][24][1] = -1;
LEDMap[14][24][0] = -1;
LEDMap[14][24][1] = -1;
LEDMap[15][24][0] = -1;
LEDMap[15][24][1] = -1;
LEDMap[16][24][0] = -1;
LEDMap[16][24][1] = -1;
LEDMap[17][24][0] = -1;
LEDMap[17][24][1] = -1;
LEDMap[18][24][0] = -1;
LEDMap[18][24][1] = -1;
LEDMap[19][24][0] = -1;
LEDMap[19][24][1] = -1;
LEDMap[20][24][0] = -1;
LEDMap[20][24][1] = -1;
LEDMap[21][24][0] = -1;
LEDMap[21][24][1] = -1;
LEDMap[22][24][0] = -1;
LEDMap[22][24][1] = -1;
LEDMap[23][24][0] = -1;
LEDMap[23][24][1] = -1;
LEDMap[24][24][0] = -1;
LEDMap[24][24][1] = -1;
LEDMap[25][24][0] = -1;
LEDMap[25][24][1] = -1;
LEDMap[26][24][0] = -1;
LEDMap[26][24][1] = -1;
LEDMap[27][24][0] = -1;
LEDMap[27][24][1] = -1;
LEDMap[28][24][0] = -1;
LEDMap[28][24][1] = -1;
LEDMap[29][24][0] = -1;
LEDMap[29][24][1] = -1;
LEDMap[30][24][0] = -1;
LEDMap[30][24][1] = -1;
LEDMap[31][24][0] = -1;
LEDMap[31][24][1] = -1;
LEDMap[32][24][0] = -1;
LEDMap[32][24][1] = -1;
LEDMap[33][24][0] = -1;
LEDMap[33][24][1] = -1;
LEDMap[34][24][0] = -1;
LEDMap[34][24][1] = -1;
LEDMap[35][24][0] = -1;
LEDMap[35][24][1] = -1;
LEDMap[36][24][0] = -1;
LEDMap[36][24][1] = -1;
LEDMap[37][24][0] = -1;
LEDMap[37][24][1] = -1;
LEDMap[38][24][0] = -1;
LEDMap[38][24][1] = -1;
LEDMap[39][24][0] = 97;
LEDMap[39][24][1] = -1;
LEDMap[40][24][0] = 96;
LEDMap[40][24][1] = -1;
LEDMap[41][24][0] = -1;
LEDMap[41][24][1] = -1;
LEDMap[42][24][0] = -1;
LEDMap[42][24][1] = -1;
LEDMap[43][24][0] = -1;
LEDMap[43][24][1] = -1;
LEDMap[0][25][0] = 37;
LEDMap[0][25][1] = -1;
LEDMap[1][25][0] = -1;
LEDMap[1][25][1] = -1;
LEDMap[2][25][0] = -1;
LEDMap[2][25][1] = -1;
LEDMap[3][25][0] = -1;
LEDMap[3][25][1] = -1;
LEDMap[4][25][0] = -1;
LEDMap[4][25][1] = -1;
LEDMap[5][25][0] = -1;
LEDMap[5][25][1] = -1;
LEDMap[6][25][0] = -1;
LEDMap[6][25][1] = -1;
LEDMap[7][25][0] = -1;
LEDMap[7][25][1] = -1;
LEDMap[8][25][0] = -1;
LEDMap[8][25][1] = -1;
LEDMap[9][25][0] = -1;
LEDMap[9][25][1] = -1;
LEDMap[10][25][0] = -1;
LEDMap[10][25][1] = -1;
LEDMap[11][25][0] = -1;
LEDMap[11][25][1] = -1;
LEDMap[12][25][0] = -1;
LEDMap[12][25][1] = -1;
LEDMap[13][25][0] = -1;
LEDMap[13][25][1] = -1;
LEDMap[14][25][0] = -1;
LEDMap[14][25][1] = -1;
LEDMap[15][25][0] = -1;
LEDMap[15][25][1] = -1;
LEDMap[16][25][0] = -1;
LEDMap[16][25][1] = -1;
LEDMap[17][25][0] = -1;
LEDMap[17][25][1] = -1;
LEDMap[18][25][0] = -1;
LEDMap[18][25][1] = -1;
LEDMap[19][25][0] = -1;
LEDMap[19][25][1] = -1;
LEDMap[20][25][0] = -1;
LEDMap[20][25][1] = -1;
LEDMap[21][25][0] = -1;
LEDMap[21][25][1] = -1;
LEDMap[22][25][0] = -1;
LEDMap[22][25][1] = -1;
LEDMap[23][25][0] = -1;
LEDMap[23][25][1] = -1;
LEDMap[24][25][0] = -1;
LEDMap[24][25][1] = -1;
LEDMap[25][25][0] = -1;
LEDMap[25][25][1] = -1;
LEDMap[26][25][0] = -1;
LEDMap[26][25][1] = -1;
LEDMap[27][25][0] = -1;
LEDMap[27][25][1] = -1;
LEDMap[28][25][0] = -1;
LEDMap[28][25][1] = -1;
LEDMap[29][25][0] = -1;
LEDMap[29][25][1] = -1;
LEDMap[30][25][0] = -1;
LEDMap[30][25][1] = -1;
LEDMap[31][25][0] = -1;
LEDMap[31][25][1] = -1;
LEDMap[32][25][0] = -1;
LEDMap[32][25][1] = -1;
LEDMap[33][25][0] = -1;
LEDMap[33][25][1] = -1;
LEDMap[34][25][0] = -1;
LEDMap[34][25][1] = -1;
LEDMap[35][25][0] = -1;
LEDMap[35][25][1] = -1;
LEDMap[36][25][0] = -1;
LEDMap[36][25][1] = -1;
LEDMap[37][25][0] = -1;
LEDMap[37][25][1] = -1;
LEDMap[38][25][0] = 98;
LEDMap[38][25][1] = -1;
LEDMap[39][25][0] = -1;
LEDMap[39][25][1] = -1;
LEDMap[40][25][0] = -1;
LEDMap[40][25][1] = -1;
LEDMap[41][25][0] = -1;
LEDMap[41][25][1] = -1;
LEDMap[42][25][0] = -1;
LEDMap[42][25][1] = -1;
LEDMap[43][25][0] = -1;
LEDMap[43][25][1] = -1;
LEDMap[0][26][0] = 36;
LEDMap[0][26][1] = -1;
LEDMap[1][26][0] = -1;
LEDMap[1][26][1] = -1;
LEDMap[2][26][0] = -1;
LEDMap[2][26][1] = -1;
LEDMap[3][26][0] = -1;
LEDMap[3][26][1] = -1;
LEDMap[4][26][0] = -1;
LEDMap[4][26][1] = -1;
LEDMap[5][26][0] = -1;
LEDMap[5][26][1] = -1;
LEDMap[6][26][0] = 122;
LEDMap[6][26][1] = -1;
LEDMap[7][26][0] = -1;
LEDMap[7][26][1] = -1;
LEDMap[8][26][0] = -1;
LEDMap[8][26][1] = -1;
LEDMap[9][26][0] = -1;
LEDMap[9][26][1] = -1;
LEDMap[10][26][0] = -1;
LEDMap[10][26][1] = -1;
LEDMap[11][26][0] = -1;
LEDMap[11][26][1] = -1;
LEDMap[12][26][0] = -1;
LEDMap[12][26][1] = -1;
LEDMap[13][26][0] = -1;
LEDMap[13][26][1] = -1;
LEDMap[14][26][0] = -1;
LEDMap[14][26][1] = -1;
LEDMap[15][26][0] = -1;
LEDMap[15][26][1] = -1;
LEDMap[16][26][0] = -1;
LEDMap[16][26][1] = -1;
LEDMap[17][26][0] = -1;
LEDMap[17][26][1] = -1;
LEDMap[18][26][0] = -1;
LEDMap[18][26][1] = -1;
LEDMap[19][26][0] = -1;
LEDMap[19][26][1] = -1;
LEDMap[20][26][0] = -1;
LEDMap[20][26][1] = -1;
LEDMap[21][26][0] = -1;
LEDMap[21][26][1] = -1;
LEDMap[22][26][0] = -1;
LEDMap[22][26][1] = -1;
LEDMap[23][26][0] = -1;
LEDMap[23][26][1] = -1;
LEDMap[24][26][0] = -1;
LEDMap[24][26][1] = -1;
LEDMap[25][26][0] = -1;
LEDMap[25][26][1] = -1;
LEDMap[26][26][0] = -1;
LEDMap[26][26][1] = -1;
LEDMap[27][26][0] = -1;
LEDMap[27][26][1] = -1;
LEDMap[28][26][0] = -1;
LEDMap[28][26][1] = -1;
LEDMap[29][26][0] = -1;
LEDMap[29][26][1] = -1;
LEDMap[30][26][0] = -1;
LEDMap[30][26][1] = -1;
LEDMap[31][26][0] = -1;
LEDMap[31][26][1] = -1;
LEDMap[32][26][0] = -1;
LEDMap[32][26][1] = -1;
LEDMap[33][26][0] = -1;
LEDMap[33][26][1] = -1;
LEDMap[34][26][0] = -1;
LEDMap[34][26][1] = -1;
LEDMap[35][26][0] = 101;
LEDMap[35][26][1] = -1;
LEDMap[36][26][0] = 100;
LEDMap[36][26][1] = -1;
LEDMap[37][26][0] = -1;
LEDMap[37][26][1] = -1;
LEDMap[38][26][0] = 99;
LEDMap[38][26][1] = -1;
LEDMap[39][26][0] = -1;
LEDMap[39][26][1] = -1;
LEDMap[40][26][0] = -1;
LEDMap[40][26][1] = -1;
LEDMap[41][26][0] = -1;
LEDMap[41][26][1] = -1;
LEDMap[42][26][0] = -1;
LEDMap[42][26][1] = -1;
LEDMap[43][26][0] = -1;
LEDMap[43][26][1] = -1;
LEDMap[0][27][0] = -1;
LEDMap[0][27][1] = -1;
LEDMap[1][27][0] = -1;
LEDMap[1][27][1] = -1;
LEDMap[2][27][0] = -1;
LEDMap[2][27][1] = -1;
LEDMap[3][27][0] = -1;
LEDMap[3][27][1] = -1;
LEDMap[4][27][0] = -1;
LEDMap[4][27][1] = -1;
LEDMap[5][27][0] = -1;
LEDMap[5][27][1] = -1;
LEDMap[6][27][0] = 121;
LEDMap[6][27][1] = -1;
LEDMap[7][27][0] = -1;
LEDMap[7][27][1] = -1;
LEDMap[8][27][0] = 120;
LEDMap[8][27][1] = -1;
LEDMap[9][27][0] = 119;
LEDMap[9][27][1] = -1;
LEDMap[10][27][0] = -1;
LEDMap[10][27][1] = -1;
LEDMap[11][27][0] = 118;
LEDMap[11][27][1] = -1;
LEDMap[12][27][0] = -1;
LEDMap[12][27][1] = -1;
LEDMap[13][27][0] = 116;
LEDMap[13][27][1] = 117;
LEDMap[14][27][0] = -1;
LEDMap[14][27][1] = -1;
LEDMap[15][27][0] = -1;
LEDMap[15][27][1] = -1;
LEDMap[16][27][0] = 115;
LEDMap[16][27][1] = -1;
LEDMap[17][27][0] = 114;
LEDMap[17][27][1] = -1;
LEDMap[18][27][0] = 113;
LEDMap[18][27][1] = -1;
LEDMap[19][27][0] = -1;
LEDMap[19][27][1] = -1;
LEDMap[20][27][0] = 112;
LEDMap[20][27][1] = -1;
LEDMap[21][27][0] = 111;
LEDMap[21][27][1] = -1;
LEDMap[22][27][0] = 110;
LEDMap[22][27][1] = -1;
LEDMap[23][27][0] = -1;
LEDMap[23][27][1] = -1;
LEDMap[24][27][0] = 109;
LEDMap[24][27][1] = -1;
LEDMap[25][27][0] = 108;
LEDMap[25][27][1] = -1;
LEDMap[26][27][0] = -1;
LEDMap[26][27][1] = -1;
LEDMap[27][27][0] = 107;
LEDMap[27][27][1] = -1;
LEDMap[28][27][0] = 106;
LEDMap[28][27][1] = -1;
LEDMap[29][27][0] = -1;
LEDMap[29][27][1] = -1;
LEDMap[30][27][0] = 105;
LEDMap[30][27][1] = -1;
LEDMap[31][27][0] = 104;
LEDMap[31][27][1] = -1;
LEDMap[32][27][0] = 103;
LEDMap[32][27][1] = -1;
LEDMap[33][27][0] = -1;
LEDMap[33][27][1] = -1;
LEDMap[34][27][0] = 102;
LEDMap[34][27][1] = -1;
LEDMap[35][27][0] = -1;
LEDMap[35][27][1] = -1;
LEDMap[36][27][0] = -1;
LEDMap[36][27][1] = -1;
LEDMap[37][27][0] = -1;
LEDMap[37][27][1] = -1;
LEDMap[38][27][0] = -1;
LEDMap[38][27][1] = -1;
LEDMap[39][27][0] = -1;
LEDMap[39][27][1] = -1;
LEDMap[40][27][0] = -1;
LEDMap[40][27][1] = -1;
LEDMap[41][27][0] = -1;
LEDMap[41][27][1] = -1;
LEDMap[42][27][0] = -1;
LEDMap[42][27][1] = -1;
LEDMap[43][27][0] = -1;
LEDMap[43][27][1] = -1;
LEDMap[0][28][0] = -1;
LEDMap[0][28][1] = -1;
LEDMap[1][28][0] = 35;
LEDMap[1][28][1] = -1;
LEDMap[2][28][0] = -1;
LEDMap[2][28][1] = -1;
LEDMap[3][28][0] = -1;
LEDMap[3][28][1] = -1;
LEDMap[4][28][0] = -1;
LEDMap[4][28][1] = -1;
LEDMap[5][28][0] = -1;
LEDMap[5][28][1] = -1;
LEDMap[6][28][0] = -1;
LEDMap[6][28][1] = -1;
LEDMap[7][28][0] = -1;
LEDMap[7][28][1] = -1;
LEDMap[8][28][0] = -1;
LEDMap[8][28][1] = -1;
LEDMap[9][28][0] = -1;
LEDMap[9][28][1] = -1;
LEDMap[10][28][0] = -1;
LEDMap[10][28][1] = -1;
LEDMap[11][28][0] = -1;
LEDMap[11][28][1] = -1;
LEDMap[12][28][0] = -1;
LEDMap[12][28][1] = -1;
LEDMap[13][28][0] = -1;
LEDMap[13][28][1] = -1;
LEDMap[14][28][0] = -1;
LEDMap[14][28][1] = -1;
LEDMap[15][28][0] = -1;
LEDMap[15][28][1] = -1;
LEDMap[16][28][0] = -1;
LEDMap[16][28][1] = -1;
LEDMap[17][28][0] = -1;
LEDMap[17][28][1] = -1;
LEDMap[18][28][0] = -1;
LEDMap[18][28][1] = -1;
LEDMap[19][28][0] = -1;
LEDMap[19][28][1] = -1;
LEDMap[20][28][0] = -1;
LEDMap[20][28][1] = -1;
LEDMap[21][28][0] = -1;
LEDMap[21][28][1] = -1;
LEDMap[22][28][0] = -1;
LEDMap[22][28][1] = -1;
LEDMap[23][28][0] = -1;
LEDMap[23][28][1] = -1;
LEDMap[24][28][0] = -1;
LEDMap[24][28][1] = -1;
LEDMap[25][28][0] = -1;
LEDMap[25][28][1] = -1;
LEDMap[26][28][0] = -1;
LEDMap[26][28][1] = -1;
LEDMap[27][28][0] = -1;
LEDMap[27][28][1] = -1;
LEDMap[28][28][0] = -1;
LEDMap[28][28][1] = -1;
LEDMap[29][28][0] = -1;
LEDMap[29][28][1] = -1;
LEDMap[30][28][0] = -1;
LEDMap[30][28][1] = -1;
LEDMap[31][28][0] = -1;
LEDMap[31][28][1] = -1;
LEDMap[32][28][0] = -1;
LEDMap[32][28][1] = -1;
LEDMap[33][28][0] = -1;
LEDMap[33][28][1] = -1;
LEDMap[34][28][0] = -1;
LEDMap[34][28][1] = -1;
LEDMap[35][28][0] = -1;
LEDMap[35][28][1] = -1;
LEDMap[36][28][0] = -1;
LEDMap[36][28][1] = -1;
LEDMap[37][28][0] = -1;
LEDMap[37][28][1] = -1;
LEDMap[38][28][0] = -1;
LEDMap[38][28][1] = -1;
LEDMap[39][28][0] = -1;
LEDMap[39][28][1] = -1;
LEDMap[40][28][0] = -1;
LEDMap[40][28][1] = -1;
LEDMap[41][28][0] = -1;
LEDMap[41][28][1] = -1;
LEDMap[42][28][0] = -1;
LEDMap[42][28][1] = -1;
LEDMap[43][28][0] = -1;
LEDMap[43][28][1] = -1;
LEDMap[0][29][0] = -1;
LEDMap[0][29][1] = -1;
LEDMap[1][29][0] = 34;
LEDMap[1][29][1] = -1;
LEDMap[2][29][0] = -1;
LEDMap[2][29][1] = -1;
LEDMap[3][29][0] = -1;
LEDMap[3][29][1] = -1;
LEDMap[4][29][0] = -1;
LEDMap[4][29][1] = -1;
LEDMap[5][29][0] = -1;
LEDMap[5][29][1] = -1;
LEDMap[6][29][0] = -1;
LEDMap[6][29][1] = -1;
LEDMap[7][29][0] = -1;
LEDMap[7][29][1] = -1;
LEDMap[8][29][0] = -1;
LEDMap[8][29][1] = -1;
LEDMap[9][29][0] = -1;
LEDMap[9][29][1] = -1;
LEDMap[10][29][0] = -1;
LEDMap[10][29][1] = -1;
LEDMap[11][29][0] = -1;
LEDMap[11][29][1] = -1;
LEDMap[12][29][0] = -1;
LEDMap[12][29][1] = -1;
LEDMap[13][29][0] = -1;
LEDMap[13][29][1] = -1;
LEDMap[14][29][0] = -1;
LEDMap[14][29][1] = -1;
LEDMap[15][29][0] = -1;
LEDMap[15][29][1] = -1;
LEDMap[16][29][0] = -1;
LEDMap[16][29][1] = -1;
LEDMap[17][29][0] = -1;
LEDMap[17][29][1] = -1;
LEDMap[18][29][0] = -1;
LEDMap[18][29][1] = -1;
LEDMap[19][29][0] = -1;
LEDMap[19][29][1] = -1;
LEDMap[20][29][0] = -1;
LEDMap[20][29][1] = -1;
LEDMap[21][29][0] = -1;
LEDMap[21][29][1] = -1;
LEDMap[22][29][0] = -1;
LEDMap[22][29][1] = -1;
LEDMap[23][29][0] = -1;
LEDMap[23][29][1] = -1;
LEDMap[24][29][0] = -1;
LEDMap[24][29][1] = -1;
LEDMap[25][29][0] = -1;
LEDMap[25][29][1] = -1;
LEDMap[26][29][0] = -1;
LEDMap[26][29][1] = -1;
LEDMap[27][29][0] = -1;
LEDMap[27][29][1] = -1;
LEDMap[28][29][0] = -1;
LEDMap[28][29][1] = -1;
LEDMap[29][29][0] = -1;
LEDMap[29][29][1] = -1;
LEDMap[30][29][0] = -1;
LEDMap[30][29][1] = -1;
LEDMap[31][29][0] = -1;
LEDMap[31][29][1] = -1;
LEDMap[32][29][0] = -1;
LEDMap[32][29][1] = -1;
LEDMap[33][29][0] = -1;
LEDMap[33][29][1] = -1;
LEDMap[34][29][0] = -1;
LEDMap[34][29][1] = -1;
LEDMap[35][29][0] = -1;
LEDMap[35][29][1] = -1;
LEDMap[36][29][0] = -1;
LEDMap[36][29][1] = -1;
LEDMap[37][29][0] = -1;
LEDMap[37][29][1] = -1;
LEDMap[38][29][0] = -1;
LEDMap[38][29][1] = -1;
LEDMap[39][29][0] = -1;
LEDMap[39][29][1] = -1;
LEDMap[40][29][0] = -1;
LEDMap[40][29][1] = -1;
LEDMap[41][29][0] = -1;
LEDMap[41][29][1] = -1;
LEDMap[42][29][0] = -1;
LEDMap[42][29][1] = -1;
LEDMap[43][29][0] = -1;
LEDMap[43][29][1] = -1;
LEDMap[0][30][0] = -1;
LEDMap[0][30][1] = -1;
LEDMap[1][30][0] = -1;
LEDMap[1][30][1] = -1;
LEDMap[2][30][0] = -1;
LEDMap[2][30][1] = -1;
LEDMap[3][30][0] = -1;
LEDMap[3][30][1] = -1;
LEDMap[4][30][0] = -1;
LEDMap[4][30][1] = -1;
LEDMap[5][30][0] = -1;
LEDMap[5][30][1] = -1;
LEDMap[6][30][0] = -1;
LEDMap[6][30][1] = -1;
LEDMap[7][30][0] = -1;
LEDMap[7][30][1] = -1;
LEDMap[8][30][0] = -1;
LEDMap[8][30][1] = -1;
LEDMap[9][30][0] = -1;
LEDMap[9][30][1] = -1;
LEDMap[10][30][0] = -1;
LEDMap[10][30][1] = -1;
LEDMap[11][30][0] = -1;
LEDMap[11][30][1] = -1;
LEDMap[12][30][0] = -1;
LEDMap[12][30][1] = -1;
LEDMap[13][30][0] = -1;
LEDMap[13][30][1] = -1;
LEDMap[14][30][0] = -1;
LEDMap[14][30][1] = -1;
LEDMap[15][30][0] = -1;
LEDMap[15][30][1] = -1;
LEDMap[16][30][0] = -1;
LEDMap[16][30][1] = -1;
LEDMap[17][30][0] = -1;
LEDMap[17][30][1] = -1;
LEDMap[18][30][0] = -1;
LEDMap[18][30][1] = -1;
LEDMap[19][30][0] = -1;
LEDMap[19][30][1] = -1;
LEDMap[20][30][0] = -1;
LEDMap[20][30][1] = -1;
LEDMap[21][30][0] = -1;
LEDMap[21][30][1] = -1;
LEDMap[22][30][0] = -1;
LEDMap[22][30][1] = -1;
LEDMap[23][30][0] = -1;
LEDMap[23][30][1] = -1;
LEDMap[24][30][0] = -1;
LEDMap[24][30][1] = -1;
LEDMap[25][30][0] = -1;
LEDMap[25][30][1] = -1;
LEDMap[26][30][0] = -1;
LEDMap[26][30][1] = -1;
LEDMap[27][30][0] = -1;
LEDMap[27][30][1] = -1;
LEDMap[28][30][0] = -1;
LEDMap[28][30][1] = -1;
LEDMap[29][30][0] = -1;
LEDMap[29][30][1] = -1;
LEDMap[30][30][0] = -1;
LEDMap[30][30][1] = -1;
LEDMap[31][30][0] = -1;
LEDMap[31][30][1] = -1;
LEDMap[32][30][0] = -1;
LEDMap[32][30][1] = -1;
LEDMap[33][30][0] = -1;
LEDMap[33][30][1] = -1;
LEDMap[34][30][0] = -1;
LEDMap[34][30][1] = -1;
LEDMap[35][30][0] = -1;
LEDMap[35][30][1] = -1;
LEDMap[36][30][0] = -1;
LEDMap[36][30][1] = -1;
LEDMap[37][30][0] = -1;
LEDMap[37][30][1] = -1;
LEDMap[38][30][0] = -1;
LEDMap[38][30][1] = -1;
LEDMap[39][30][0] = -1;
LEDMap[39][30][1] = -1;
LEDMap[40][30][0] = -1;
LEDMap[40][30][1] = -1;
LEDMap[41][30][0] = -1;
LEDMap[41][30][1] = -1;
LEDMap[42][30][0] = -1;
LEDMap[42][30][1] = -1;
LEDMap[43][30][0] = -1;
LEDMap[43][30][1] = -1;
LEDMap[0][31][0] = -1;
LEDMap[0][31][1] = -1;
LEDMap[1][31][0] = 33;
LEDMap[1][31][1] = -1;
LEDMap[2][31][0] = -1;
LEDMap[2][31][1] = -1;
LEDMap[3][31][0] = -1;
LEDMap[3][31][1] = -1;
LEDMap[4][31][0] = -1;
LEDMap[4][31][1] = -1;
LEDMap[5][31][0] = -1;
LEDMap[5][31][1] = -1;
LEDMap[6][31][0] = -1;
LEDMap[6][31][1] = -1;
LEDMap[7][31][0] = -1;
LEDMap[7][31][1] = -1;
LEDMap[8][31][0] = -1;
LEDMap[8][31][1] = -1;
LEDMap[9][31][0] = -1;
LEDMap[9][31][1] = -1;
LEDMap[10][31][0] = -1;
LEDMap[10][31][1] = -1;
LEDMap[11][31][0] = -1;
LEDMap[11][31][1] = -1;
LEDMap[12][31][0] = -1;
LEDMap[12][31][1] = -1;
LEDMap[13][31][0] = -1;
LEDMap[13][31][1] = -1;
LEDMap[14][31][0] = -1;
LEDMap[14][31][1] = -1;
LEDMap[15][31][0] = -1;
LEDMap[15][31][1] = -1;
LEDMap[16][31][0] = -1;
LEDMap[16][31][1] = -1;
LEDMap[17][31][0] = -1;
LEDMap[17][31][1] = -1;
LEDMap[18][31][0] = -1;
LEDMap[18][31][1] = -1;
LEDMap[19][31][0] = -1;
LEDMap[19][31][1] = -1;
LEDMap[20][31][0] = -1;
LEDMap[20][31][1] = -1;
LEDMap[21][31][0] = -1;
LEDMap[21][31][1] = -1;
LEDMap[22][31][0] = -1;
LEDMap[22][31][1] = -1;
LEDMap[23][31][0] = -1;
LEDMap[23][31][1] = -1;
LEDMap[24][31][0] = -1;
LEDMap[24][31][1] = -1;
LEDMap[25][31][0] = -1;
LEDMap[25][31][1] = -1;
LEDMap[26][31][0] = -1;
LEDMap[26][31][1] = -1;
LEDMap[27][31][0] = -1;
LEDMap[27][31][1] = -1;
LEDMap[28][31][0] = -1;
LEDMap[28][31][1] = -1;
LEDMap[29][31][0] = -1;
LEDMap[29][31][1] = -1;
LEDMap[30][31][0] = -1;
LEDMap[30][31][1] = -1;
LEDMap[31][31][0] = -1;
LEDMap[31][31][1] = -1;
LEDMap[32][31][0] = -1;
LEDMap[32][31][1] = -1;
LEDMap[33][31][0] = -1;
LEDMap[33][31][1] = -1;
LEDMap[34][31][0] = -1;
LEDMap[34][31][1] = -1;
LEDMap[35][31][0] = -1;
LEDMap[35][31][1] = -1;
LEDMap[36][31][0] = -1;
LEDMap[36][31][1] = -1;
LEDMap[37][31][0] = -1;
LEDMap[37][31][1] = -1;
LEDMap[38][31][0] = -1;
LEDMap[38][31][1] = -1;
LEDMap[39][31][0] = -1;
LEDMap[39][31][1] = -1;
LEDMap[40][31][0] = -1;
LEDMap[40][31][1] = -1;
LEDMap[41][31][0] = -1;
LEDMap[41][31][1] = -1;
LEDMap[42][31][0] = -1;
LEDMap[42][31][1] = -1;
LEDMap[43][31][0] = 1;
LEDMap[43][31][1] = -1;
LEDMap[0][32][0] = 32;
LEDMap[0][32][1] = -1;
LEDMap[1][32][0] = -1;
LEDMap[1][32][1] = -1;
LEDMap[2][32][0] = -1;
LEDMap[2][32][1] = -1;
LEDMap[3][32][0] = -1;
LEDMap[3][32][1] = -1;
LEDMap[4][32][0] = -1;
LEDMap[4][32][1] = -1;
LEDMap[5][32][0] = -1;
LEDMap[5][32][1] = -1;
LEDMap[6][32][0] = -1;
LEDMap[6][32][1] = -1;
LEDMap[7][32][0] = -1;
LEDMap[7][32][1] = -1;
LEDMap[8][32][0] = -1;
LEDMap[8][32][1] = -1;
LEDMap[9][32][0] = -1;
LEDMap[9][32][1] = -1;
LEDMap[10][32][0] = -1;
LEDMap[10][32][1] = -1;
LEDMap[11][32][0] = -1;
LEDMap[11][32][1] = -1;
LEDMap[12][32][0] = -1;
LEDMap[12][32][1] = -1;
LEDMap[13][32][0] = -1;
LEDMap[13][32][1] = -1;
LEDMap[14][32][0] = -1;
LEDMap[14][32][1] = -1;
LEDMap[15][32][0] = -1;
LEDMap[15][32][1] = -1;
LEDMap[16][32][0] = -1;
LEDMap[16][32][1] = -1;
LEDMap[17][32][0] = -1;
LEDMap[17][32][1] = -1;
LEDMap[18][32][0] = -1;
LEDMap[18][32][1] = -1;
LEDMap[19][32][0] = -1;
LEDMap[19][32][1] = -1;
LEDMap[20][32][0] = -1;
LEDMap[20][32][1] = -1;
LEDMap[21][32][0] = -1;
LEDMap[21][32][1] = -1;
LEDMap[22][32][0] = 16;
LEDMap[22][32][1] = -1;
LEDMap[23][32][0] = 15;
LEDMap[23][32][1] = -1;
LEDMap[24][32][0] = -1;
LEDMap[24][32][1] = -1;
LEDMap[25][32][0] = 14;
LEDMap[25][32][1] = -1;
LEDMap[26][32][0] = 13;
LEDMap[26][32][1] = -1;
LEDMap[27][32][0] = -1;
LEDMap[27][32][1] = -1;
LEDMap[28][32][0] = 12;
LEDMap[28][32][1] = -1;
LEDMap[29][32][0] = 11;
LEDMap[29][32][1] = -1;
LEDMap[30][32][0] = 10;
LEDMap[30][32][1] = -1;
LEDMap[31][32][0] = 9;
LEDMap[31][32][1] = -1;
LEDMap[32][32][0] = -1;
LEDMap[32][32][1] = -1;
LEDMap[33][32][0] = 8;
LEDMap[33][32][1] = -1;
LEDMap[34][32][0] = -1;
LEDMap[34][32][1] = -1;
LEDMap[35][32][0] = 7;
LEDMap[35][32][1] = -1;
LEDMap[36][32][0] = 6;
LEDMap[36][32][1] = -1;
LEDMap[37][32][0] = -1;
LEDMap[37][32][1] = -1;
LEDMap[38][32][0] = 5;
LEDMap[38][32][1] = 5;
LEDMap[39][32][0] = 4;
LEDMap[39][32][1] = -1;
LEDMap[40][32][0] = 3;
LEDMap[40][32][1] = -1;
LEDMap[41][32][0] = 2;
LEDMap[41][32][1] = -1;
LEDMap[42][32][0] = -1;
LEDMap[42][32][1] = -1;
LEDMap[43][32][0] = -1;
LEDMap[43][32][1] = -1;
LEDMap[0][33][0] = 31;
LEDMap[0][33][1] = -1;
LEDMap[1][33][0] = -1;
LEDMap[1][33][1] = -1;
LEDMap[2][33][0] = -1;
LEDMap[2][33][1] = -1;
LEDMap[3][33][0] = 29;
LEDMap[3][33][1] = -1;
LEDMap[4][33][0] = -1;
LEDMap[4][33][1] = -1;
LEDMap[5][33][0] = 28;
LEDMap[5][33][1] = -1;
LEDMap[6][33][0] = 27;
LEDMap[6][33][1] = -1;
LEDMap[7][33][0] = -1;
LEDMap[7][33][1] = -1;
LEDMap[8][33][0] = 26;
LEDMap[8][33][1] = -1;
LEDMap[9][33][0] = 25;
LEDMap[9][33][1] = -1;
LEDMap[10][33][0] = -1;
LEDMap[10][33][1] = -1;
LEDMap[11][33][0] = 24;
LEDMap[11][33][1] = -1;
LEDMap[12][33][0] = 23;
LEDMap[12][33][1] = -1;
LEDMap[13][33][0] = 22;
LEDMap[13][33][1] = -1;
LEDMap[14][33][0] = -1;
LEDMap[14][33][1] = -1;
LEDMap[15][33][0] = 21;
LEDMap[15][33][1] = -1;
LEDMap[16][33][0] = 20;
LEDMap[16][33][1] = -1;
LEDMap[17][33][0] = -1;
LEDMap[17][33][1] = -1;
LEDMap[18][33][0] = 19;
LEDMap[18][33][1] = -1;
LEDMap[19][33][0] = 18;
LEDMap[19][33][1] = -1;
LEDMap[20][33][0] = 17;
LEDMap[20][33][1] = -1;
LEDMap[21][33][0] = -1;
LEDMap[21][33][1] = -1;
LEDMap[22][33][0] = -1;
LEDMap[22][33][1] = -1;
LEDMap[23][33][0] = -1;
LEDMap[23][33][1] = -1;
LEDMap[24][33][0] = -1;
LEDMap[24][33][1] = -1;
LEDMap[25][33][0] = -1;
LEDMap[25][33][1] = -1;
LEDMap[26][33][0] = -1;
LEDMap[26][33][1] = -1;
LEDMap[27][33][0] = -1;
LEDMap[27][33][1] = -1;
LEDMap[28][33][0] = -1;
LEDMap[28][33][1] = -1;
LEDMap[29][33][0] = -1;
LEDMap[29][33][1] = -1;
LEDMap[30][33][0] = -1;
LEDMap[30][33][1] = -1;
LEDMap[31][33][0] = -1;
LEDMap[31][33][1] = -1;
LEDMap[32][33][0] = -1;
LEDMap[32][33][1] = -1;
LEDMap[33][33][0] = -1;
LEDMap[33][33][1] = -1;
LEDMap[34][33][0] = -1;
LEDMap[34][33][1] = -1;
LEDMap[35][33][0] = -1;
LEDMap[35][33][1] = -1;
LEDMap[36][33][0] = -1;
LEDMap[36][33][1] = -1;
LEDMap[37][33][0] = -1;
LEDMap[37][33][1] = -1;
LEDMap[38][33][0] = -1;
LEDMap[38][33][1] = -1;
LEDMap[39][33][0] = -1;
LEDMap[39][33][1] = -1;
LEDMap[40][33][0] = -1;
LEDMap[40][33][1] = -1;
LEDMap[41][33][0] = -1;
LEDMap[41][33][1] = -1;
LEDMap[42][33][0] = -1;
LEDMap[42][33][1] = -1;
LEDMap[43][33][0] = -1;
LEDMap[43][33][1] = -1;
}
