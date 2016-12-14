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
#define ANIM2

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 255

int PIR1Pin = 4; // TOP
int PIR2Pin = 5; // RIGHT
int PIR3Pin = 3; // LEFT
int PIR4Pin = 2; // Bottom

int pausePin = 14;

void setupPausePin()
{
  pinMode(pausePin, INPUT);
}

int readPausePin()
{
  Serial.println(digitalRead(pausePin));
  return digitalRead(pausePin);
}

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

int xSize = 18;
int ySize = 49;
int maxLEDList = 2;
int LEDMap[18][49][2];
MatrixDraw draw(xSize, ySize, 9);

// Params for width and height
const uint8_t kMatrixWidth = xSize;
const uint8_t kMatrixHeight = ySize;

// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;


#define RGBW
#define NUM_LEDS 400

#ifdef RGBW
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_RGBW + NEO_KHZ800);
#else
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
#endif

void DrawShape(float centerY, float size, float spread, float brightness)
{
    if(brightness > 1.0)
      brightness = 1.0;
    else if(brightness < 0.0)
      brightness = 0.0;

    int maxX = xSize - 1;
    int maxY = ySize - 1;
    int maxLength = 10;
    int curLength = size*maxLength;
    int yPosCenter = centerY*maxY;
    int startY = yPosCenter - curLength/2;
    int endY = yPosCenter + curLength/2;
    int maxSpread = 17;
    int filterKernel = spread*maxSpread;
    int shapeBrightness = brightness*255;

    if(filterKernel%2 == 0)
      filterKernel++;
    if(filterKernel > maxSpread)
      filterKernel = maxSpread;

    if(endY > maxY)
      endY = maxY;
    else if(endY < 0)
      endY = 0;

    if(startY < 0)
      startY = 0;
    else if(startY > maxY)
      startY = maxY;

    draw.DrawRectangle(0, startY, maxX, endY, shapeBrightness);
    draw.Fill(maxX/2,yPosCenter, shapeBrightness);
    draw.MeanFilter(9);
}

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

#define STATE1TIME 0
#define STATE2TIME STATE1TIME + 1600 // END RISE UP START FLUTTER
#define STATE3TIME STATE2TIME + 50 
#define STATE4TIME STATE3TIME + 50 
#define STATE5TIME STATE4TIME + 100
#define STATE6TIME STATE5TIME + 100
#define STATE7TIME STATE6TIME + 200
#define STATE8TIME STATE7TIME + 200
#define STATE9TIME STATE8TIME + 400// End Flutter one START PULSE 2
#define STATE10TIME STATE9TIME + 800  // End PULSE 2 start flutter 2
#define STATE11TIME STATE10TIME + 50 
#define STATE12TIME STATE11TIME + 50 
#define STATE13TIME STATE12TIME + 100// 4300
#define STATE14TIME STATE13TIME + 100 
#define STATE15TIME STATE14TIME + 200 
#define STATE16TIME STATE15TIME + 200 
#define STATE17TIME STATE16TIME + 400 
#define STATE18TIME STATE17TIME + 400  // End Flutter 2 Start PULSE 3
#define STATE19TIME STATE18TIME + 600  // End PULSE 3 start Flutter 3
#define STATE20TIME STATE19TIME + 50 
#define STATE21TIME STATE20TIME + 50 
#define STATE22TIME STATE21TIME + 100 
#define STATE23TIME STATE22TIME + 100 
#define STATE24TIME STATE23TIME + 200 
#define STATE25TIME STATE24TIME + 200 
#define STATE26TIME STATE25TIME + 400 
#define STATE27TIME STATE26TIME + 400 
#define STATE28TIME STATE27TIME + 600  //Start the hold and tremble
#define STATE29TIME STATE28TIME + 1000  
#define MAX_TIME STATE29TIME + 500 
#define MAXQI 0.2
#define MAXQ 0.5
#define MAXQ2 0.3

void ReadPIRSensors()
{
	PIR1State = digitalRead(PIR1Pin);
  PIR2State = digitalRead(PIR2Pin);

  if(PIR1State || PIR2State)
  {
    PIR1State = 1;
    PIR2State = 0;
  }

	PIR3State = digitalRead(PIR3Pin);
	PIR4State = digitalRead(PIR4Pin);

  if(PIR3State || PIR4State)
  {
    PIR3State = 1;
    PIR4State = 0;
  }

  // PIR1State = 0;
  // PIR2State = 0;
  // PIR3State = 0;
  // PIR4State = 0;

  long curMillis = GetPIRStartTime(PIR1State, lastPIR1State);
  if(curMillis > 0 && curMillis - PIR1StartTime > MAX_TIME)
    PIR1StartTime = curMillis;

  curMillis = GetPIRStartTime(PIR2State, lastPIR2State);
  if(curMillis > 0 && curMillis - PIR2StartTime > MAX_TIME)
    PIR2StartTime = curMillis;

  curMillis = GetPIRStartTime(PIR3State, lastPIR3State);
  if(curMillis > 0 && curMillis - PIR3StartTime > MAX_TIME)
    PIR3StartTime = curMillis;

  curMillis = GetPIRStartTime(PIR4State, lastPIR4State);
  if(curMillis > 0 && curMillis - PIR4StartTime > MAX_TIME)
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
#define STATE2TIME STATE1TIME + 300
#define STATE3TIME STATE2TIME + 300
#define STATE4TIME STATE3TIME + 300 
#define STATE5TIME STATE4TIME + 300 
#define STATE6TIME STATE5TIME + 300   
#define STATE7TIME STATE6TIME + 300   
#define STATE8TIME STATE7TIME + 300   
#define STATE9TIME STATE8TIME + 300   
#define STATE10TIME STATE9TIME + 300  
#define STATE11TIME STATE10TIME + 300 
#define STATE12TIME STATE11TIME + 300 
#define STATE13TIME STATE12TIME + 300 
#define STATE14TIME STATE13TIME + 300  //Begin stand Still
#define STATE15TIME STATE14TIME + 300  //BEGIN Pulse
#define STATE16TIME STATE15TIME + 300 
#define STATE17TIME STATE16TIME + 300 
#define STATE18TIME STATE17TIME + 8000
#define STATE19TIME STATE18TIME + 500 
#define STATE20TIME STATE19TIME + 500 //Light state to fade
#define STATE21TIME STATE20TIME + 500 
#define STATE22TIME STATE21TIME + 500 
#define STATE23TIME STATE22TIME + 500 
#define MAX_TIME STATE22TIME + 1000   

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



long lastMillis = 0;
float binaryValue = 0.1;
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
    PIRVal -= GetPulseSpeed(STATE29TIME, MAX_TIME, lastPIRValue, 0.0);
    //PIRVal = 0;
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
    PIRVal += GetPulseSpeed(STATE18TIME, STATE19TIME, MAXQ, MAXQ2);
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
    PIRVal += GetPulseSpeed(STATE9TIME, STATE10TIME, MAXQ2, MAXQI);
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
    PIRVal += GetPulseSpeed(STATE1TIME, STATE2TIME, MAXQI, 0);
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
#define STATE2TIME STATE1TIME + 600   
#define STATE3TIME STATE2TIME + 600   
#define STATE4TIME STATE3TIME + 600   
#define STATE5TIME STATE4TIME + 600   
#define STATE6TIME STATE5TIME + 200  
#define STATE7TIME STATE6TIME + 200   
#define STATE8TIME STATE7TIME + 600   
#define STATE9TIME STATE8TIME + 600   
#define STATE10TIME STATE9TIME + 600  
#define STATE11TIME STATE10TIME + 600 
#define STATE12TIME STATE11TIME + 600 
#define STATE13TIME STATE12TIME + 600 
#define STATE14TIME STATE13TIME + 600  //Begin stand Still
#define STATE15TIME STATE14TIME + 600  //BEGIN Pulse
#define STATE16TIME STATE15TIME + 600 
#define STATE17TIME STATE16TIME + 600 
#define STATE18TIME STATE17TIME + 600 
#define STATE19TIME STATE18TIME + 600   //Light state to fade
#define STATE20TIME STATE19TIME + 3200
#define STATE21TIME STATE20TIME + 600 
#define STATE22TIME STATE21TIME + 600 
#define STATE23TIME STATE22TIME + 600 
#define STATE24TIME STATE23TIME + 3000
#define MAX_TIME STATE24TIME + 3000   

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
float lastQ11;
float lastQ12;
float lastQ21;
float lastQ22;
void GetBilinearValues(float &Q11, float &Q12, float &Q21, float &Q22)
{
  Serial.println(readPausePin());
  if(readPausePin() == 0)
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
    // Q11 - Up LEFT
    // Q12 - Up Right
    // Q21 - Bottom Left
    // Q22 - Bottom Right
    //Q11 = (PIR2Val + PIR3Val) / 2;


    lastQ11 = Q11 = (PIR1Val + PIR3Val) / 2;
    lastQ12 = Q12 = (PIR1Val + PIR2Val) / 2;
    lastQ21 = Q21 = (PIR4Val + PIR3Val) / 2;
    lastQ22 = Q22 = (PIR2Val + PIR4Val) / 2;
  }
  else
  {
    Q11 = lastQ11;
    Q12 = lastQ12;
    Q22 = lastQ22;
    Q21 = lastQ21;
  }
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

    if(millis() - PIR1StartTime > STATE29TIME && millis() - PIR1StartTime < MAX_TIME)
    {

      float maxTime = MAX_TIME;
      float stateTime = STATE29TIME;
      float move = maxTime - stateTime;
      float curMillis = millis();
      float getPos = curMillis - (float)PIR1StartTime - stateTime;
      float curPos = getPos/move;
      DrawShape(curPos, 1.0, 1.0, 1.0);

      // Serial.print(maxTime);
      // Serial.print(" ");
      // Serial.print(stateTime);
      // Serial.print(" ");
      // Serial.print(move);
      // Serial.print(" ");
      // Serial.print(PIR1StartTime);
      // Serial.print(" ");
      // Serial.print(getPos);
      // Serial.print(" ");
      // Serial.println(curPos);
    }
    if(millis() - PIR2StartTime > STATE29TIME && millis() - PIR3StartTime < MAX_TIME)
    {
      float maxTime = MAX_TIME;
      float stateTime = STATE29TIME;
      float move = maxTime - stateTime;
      float curMillis = millis();
      float getPos = curMillis - (float)PIR1StartTime - stateTime;
      float curPos = getPos/move;
      DrawShape(curPos, 1.0, 1.0, 1.0);
    }
    if(millis() - PIR3StartTime > STATE29TIME && millis() - PIR3StartTime < MAX_TIME)
    {
      float maxTime = MAX_TIME;
      float stateTime = STATE29TIME;
      float move = maxTime - stateTime;
      float curMillis = millis();
      float getPos = curMillis - (float)PIR1StartTime - stateTime;
      float curPos = getPos/move;
      DrawShape(1.0-curPos, 1.0, 1.0, 1.0);
    }
    if(millis() - PIR3StartTime > STATE29TIME && millis() - PIR3StartTime < MAX_TIME)
    {
      float maxTime = MAX_TIME;
      float stateTime = STATE29TIME;
      float move = maxTime - stateTime;
      float curMillis = millis();
      float getPos = curMillis - (float)PIR1StartTime - stateTime;
      float curPos = getPos/move;
      DrawShape(1.0-curPos, 1.0, 1.0, 1.0);
    }

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

  setupPausePin();
  setupPIR();
  initArray();
  
  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show();
}

void initArray(){
LEDMap[0][0][0] = 69;
LEDMap[0][0][1] = -1;
LEDMap[1][0][0] = 70;
LEDMap[1][0][1] = 71;
LEDMap[2][0][0] = 72;
LEDMap[2][0][1] = -1;
LEDMap[3][0][0] = 73;
LEDMap[3][0][1] = -1;
LEDMap[4][0][0] = -1;
LEDMap[4][0][1] = -1;
LEDMap[5][0][0] = -1;
LEDMap[5][0][1] = -1;
LEDMap[6][0][0] = -1;
LEDMap[6][0][1] = -1;
LEDMap[7][0][0] = -1;
LEDMap[7][0][1] = -1;
LEDMap[8][0][0] = 220;
LEDMap[8][0][1] = -1;
LEDMap[9][0][0] = 221;
LEDMap[9][0][1] = -1;
LEDMap[10][0][0] = 222;
LEDMap[10][0][1] = -1;
LEDMap[11][0][0] = 223;
LEDMap[11][0][1] = -1;
LEDMap[12][0][0] = -1;
LEDMap[12][0][1] = -1;
LEDMap[13][0][0] = -1;
LEDMap[13][0][1] = -1;
LEDMap[14][0][0] = -1;
LEDMap[14][0][1] = -1;
LEDMap[15][0][0] = -1;
LEDMap[15][0][1] = -1;
LEDMap[16][0][0] = -1;
LEDMap[16][0][1] = -1;
LEDMap[17][0][0] = -1;
LEDMap[17][0][1] = -1;
LEDMap[0][1][0] = -1;
LEDMap[0][1][1] = -1;
LEDMap[1][1][0] = -1;
LEDMap[1][1][1] = -1;
LEDMap[2][1][0] = -1;
LEDMap[2][1][1] = -1;
LEDMap[3][1][0] = 74;
LEDMap[3][1][1] = -1;
LEDMap[4][1][0] = -1;
LEDMap[4][1][1] = -1;
LEDMap[5][1][0] = -1;
LEDMap[5][1][1] = -1;
LEDMap[6][1][0] = -1;
LEDMap[6][1][1] = -1;
LEDMap[7][1][0] = 218;
LEDMap[7][1][1] = -1;
LEDMap[8][1][0] = -1;
LEDMap[8][1][1] = -1;
LEDMap[9][1][0] = -1;
LEDMap[9][1][1] = -1;
LEDMap[10][1][0] = -1;
LEDMap[10][1][1] = -1;
LEDMap[11][1][0] = 224;
LEDMap[11][1][1] = 225;
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
LEDMap[0][2][0] = 66;
LEDMap[0][2][1] = 67;
LEDMap[1][2][0] = -1;
LEDMap[1][2][1] = -1;
LEDMap[2][2][0] = -1;
LEDMap[2][2][1] = -1;
LEDMap[3][2][0] = 75;
LEDMap[3][2][1] = 76;
LEDMap[4][2][0] = -1;
LEDMap[4][2][1] = -1;
LEDMap[5][2][0] = -1;
LEDMap[5][2][1] = -1;
LEDMap[6][2][0] = -1;
LEDMap[6][2][1] = -1;
LEDMap[7][2][0] = 217;
LEDMap[7][2][1] = -1;
LEDMap[8][2][0] = -1;
LEDMap[8][2][1] = -1;
LEDMap[9][2][0] = -1;
LEDMap[9][2][1] = -1;
LEDMap[10][2][0] = -1;
LEDMap[10][2][1] = -1;
LEDMap[11][2][0] = 226;
LEDMap[11][2][1] = -1;
LEDMap[12][2][0] = -1;
LEDMap[12][2][1] = -1;
LEDMap[13][2][0] = -1;
LEDMap[13][2][1] = -1;
LEDMap[14][2][0] = -1;
LEDMap[14][2][1] = -1;
LEDMap[15][2][0] = 367;
LEDMap[15][2][1] = -1;
LEDMap[16][2][0] = -1;
LEDMap[16][2][1] = -1;
LEDMap[17][2][0] = -1;
LEDMap[17][2][1] = -1;
LEDMap[0][3][0] = 65;
LEDMap[0][3][1] = -1;
LEDMap[1][3][0] = -1;
LEDMap[1][3][1] = -1;
LEDMap[2][3][0] = -1;
LEDMap[2][3][1] = -1;
LEDMap[3][3][0] = 77;
LEDMap[3][3][1] = -1;
LEDMap[4][3][0] = -1;
LEDMap[4][3][1] = -1;
LEDMap[5][3][0] = -1;
LEDMap[5][3][1] = -1;
LEDMap[6][3][0] = -1;
LEDMap[6][3][1] = -1;
LEDMap[7][3][0] = 215;
LEDMap[7][3][1] = 216;
LEDMap[8][3][0] = -1;
LEDMap[8][3][1] = -1;
LEDMap[9][3][0] = -1;
LEDMap[9][3][1] = -1;
LEDMap[10][3][0] = -1;
LEDMap[10][3][1] = -1;
LEDMap[11][3][0] = 227;
LEDMap[11][3][1] = 228;
LEDMap[12][3][0] = -1;
LEDMap[12][3][1] = -1;
LEDMap[13][3][0] = -1;
LEDMap[13][3][1] = -1;
LEDMap[14][3][0] = -1;
LEDMap[14][3][1] = -1;
LEDMap[15][3][0] = 366;
LEDMap[15][3][1] = -1;
LEDMap[16][3][0] = -1;
LEDMap[16][3][1] = -1;
LEDMap[17][3][0] = -1;
LEDMap[17][3][1] = -1;
LEDMap[0][4][0] = 64;
LEDMap[0][4][1] = -1;
LEDMap[1][4][0] = -1;
LEDMap[1][4][1] = -1;
LEDMap[2][4][0] = -1;
LEDMap[2][4][1] = -1;
LEDMap[3][4][0] = 78;
LEDMap[3][4][1] = -1;
LEDMap[4][4][0] = -1;
LEDMap[4][4][1] = -1;
LEDMap[5][4][0] = -1;
LEDMap[5][4][1] = -1;
LEDMap[6][4][0] = -1;
LEDMap[6][4][1] = -1;
LEDMap[7][4][0] = 214;
LEDMap[7][4][1] = -1;
LEDMap[8][4][0] = -1;
LEDMap[8][4][1] = -1;
LEDMap[9][4][0] = -1;
LEDMap[9][4][1] = -1;
LEDMap[10][4][0] = -1;
LEDMap[10][4][1] = -1;
LEDMap[11][4][0] = 229;
LEDMap[11][4][1] = -1;
LEDMap[12][4][0] = -1;
LEDMap[12][4][1] = -1;
LEDMap[13][4][0] = -1;
LEDMap[13][4][1] = -1;
LEDMap[14][4][0] = -1;
LEDMap[14][4][1] = -1;
LEDMap[15][4][0] = 364;
LEDMap[15][4][1] = 365;
LEDMap[16][4][0] = -1;
LEDMap[16][4][1] = -1;
LEDMap[17][4][0] = -1;
LEDMap[17][4][1] = -1;
LEDMap[0][5][0] = 62;
LEDMap[0][5][1] = 63;
LEDMap[1][5][0] = -1;
LEDMap[1][5][1] = -1;
LEDMap[2][5][0] = -1;
LEDMap[2][5][1] = -1;
LEDMap[3][5][0] = 79;
LEDMap[3][5][1] = 80;
LEDMap[4][5][0] = -1;
LEDMap[4][5][1] = -1;
LEDMap[5][5][0] = -1;
LEDMap[5][5][1] = -1;
LEDMap[6][5][0] = -1;
LEDMap[6][5][1] = -1;
LEDMap[7][5][0] = 212;
LEDMap[7][5][1] = 213;
LEDMap[8][5][0] = -1;
LEDMap[8][5][1] = -1;
LEDMap[9][5][0] = -1;
LEDMap[9][5][1] = -1;
LEDMap[10][5][0] = -1;
LEDMap[10][5][1] = -1;
LEDMap[11][5][0] = 230;
LEDMap[11][5][1] = 231;
LEDMap[12][5][0] = -1;
LEDMap[12][5][1] = -1;
LEDMap[13][5][0] = -1;
LEDMap[13][5][1] = -1;
LEDMap[14][5][0] = -1;
LEDMap[14][5][1] = -1;
LEDMap[15][5][0] = 363;
LEDMap[15][5][1] = -1;
LEDMap[16][5][0] = -1;
LEDMap[16][5][1] = -1;
LEDMap[17][5][0] = -1;
LEDMap[17][5][1] = -1;
LEDMap[0][6][0] = 61;
LEDMap[0][6][1] = -1;
LEDMap[1][6][0] = -1;
LEDMap[1][6][1] = -1;
LEDMap[2][6][0] = -1;
LEDMap[2][6][1] = -1;
LEDMap[3][6][0] = 81;
LEDMap[3][6][1] = -1;
LEDMap[4][6][0] = -1;
LEDMap[4][6][1] = -1;
LEDMap[5][6][0] = -1;
LEDMap[5][6][1] = -1;
LEDMap[6][6][0] = -1;
LEDMap[6][6][1] = -1;
LEDMap[7][6][0] = 211;
LEDMap[7][6][1] = -1;
LEDMap[8][6][0] = -1;
LEDMap[8][6][1] = -1;
LEDMap[9][6][0] = -1;
LEDMap[9][6][1] = -1;
LEDMap[10][6][0] = -1;
LEDMap[10][6][1] = -1;
LEDMap[11][6][0] = 232;
LEDMap[11][6][1] = -1;
LEDMap[12][6][0] = -1;
LEDMap[12][6][1] = -1;
LEDMap[13][6][0] = -1;
LEDMap[13][6][1] = -1;
LEDMap[14][6][0] = -1;
LEDMap[14][6][1] = -1;
LEDMap[15][6][0] = 361;
LEDMap[15][6][1] = 362;
LEDMap[16][6][0] = -1;
LEDMap[16][6][1] = -1;
LEDMap[17][6][0] = -1;
LEDMap[17][6][1] = -1;
LEDMap[0][7][0] = 59;
LEDMap[0][7][1] = 60;
LEDMap[1][7][0] = -1;
LEDMap[1][7][1] = -1;
LEDMap[2][7][0] = -1;
LEDMap[2][7][1] = -1;
LEDMap[3][7][0] = 82;
LEDMap[3][7][1] = 83;
LEDMap[4][7][0] = -1;
LEDMap[4][7][1] = -1;
LEDMap[5][7][0] = -1;
LEDMap[5][7][1] = -1;
LEDMap[6][7][0] = -1;
LEDMap[6][7][1] = -1;
LEDMap[7][7][0] = 209;
LEDMap[7][7][1] = 210;
LEDMap[8][7][0] = -1;
LEDMap[8][7][1] = -1;
LEDMap[9][7][0] = -1;
LEDMap[9][7][1] = -1;
LEDMap[10][7][0] = -1;
LEDMap[10][7][1] = -1;
LEDMap[11][7][0] = 233;
LEDMap[11][7][1] = -1;
LEDMap[12][7][0] = -1;
LEDMap[12][7][1] = -1;
LEDMap[13][7][0] = -1;
LEDMap[13][7][1] = -1;
LEDMap[14][7][0] = -1;
LEDMap[14][7][1] = -1;
LEDMap[15][7][0] = 360;
LEDMap[15][7][1] = -1;
LEDMap[16][7][0] = -1;
LEDMap[16][7][1] = -1;
LEDMap[17][7][0] = -1;
LEDMap[17][7][1] = -1;
LEDMap[0][8][0] = 58;
LEDMap[0][8][1] = -1;
LEDMap[1][8][0] = -1;
LEDMap[1][8][1] = -1;
LEDMap[2][8][0] = -1;
LEDMap[2][8][1] = -1;
LEDMap[3][8][0] = 84;
LEDMap[3][8][1] = -1;
LEDMap[4][8][0] = -1;
LEDMap[4][8][1] = -1;
LEDMap[5][8][0] = -1;
LEDMap[5][8][1] = -1;
LEDMap[6][8][0] = -1;
LEDMap[6][8][1] = -1;
LEDMap[7][8][0] = 208;
LEDMap[7][8][1] = -1;
LEDMap[8][8][0] = -1;
LEDMap[8][8][1] = -1;
LEDMap[9][8][0] = -1;
LEDMap[9][8][1] = -1;
LEDMap[10][8][0] = -1;
LEDMap[10][8][1] = -1;
LEDMap[11][8][0] = 234;
LEDMap[11][8][1] = 235;
LEDMap[12][8][0] = -1;
LEDMap[12][8][1] = -1;
LEDMap[13][8][0] = -1;
LEDMap[13][8][1] = -1;
LEDMap[14][8][0] = -1;
LEDMap[14][8][1] = -1;
LEDMap[15][8][0] = 358;
LEDMap[15][8][1] = 359;
LEDMap[16][8][0] = -1;
LEDMap[16][8][1] = -1;
LEDMap[17][8][0] = -1;
LEDMap[17][8][1] = -1;
LEDMap[0][9][0] = 56;
LEDMap[0][9][1] = 57;
LEDMap[1][9][0] = -1;
LEDMap[1][9][1] = -1;
LEDMap[2][9][0] = -1;
LEDMap[2][9][1] = -1;
LEDMap[3][9][0] = 85;
LEDMap[3][9][1] = 86;
LEDMap[4][9][0] = -1;
LEDMap[4][9][1] = -1;
LEDMap[5][9][0] = -1;
LEDMap[5][9][1] = -1;
LEDMap[6][9][0] = -1;
LEDMap[6][9][1] = -1;
LEDMap[7][9][0] = 207;
LEDMap[7][9][1] = -1;
LEDMap[8][9][0] = -1;
LEDMap[8][9][1] = -1;
LEDMap[9][9][0] = -1;
LEDMap[9][9][1] = -1;
LEDMap[10][9][0] = -1;
LEDMap[10][9][1] = -1;
LEDMap[11][9][0] = 236;
LEDMap[11][9][1] = -1;
LEDMap[12][9][0] = -1;
LEDMap[12][9][1] = -1;
LEDMap[13][9][0] = -1;
LEDMap[13][9][1] = -1;
LEDMap[14][9][0] = -1;
LEDMap[14][9][1] = -1;
LEDMap[15][9][0] = 357;
LEDMap[15][9][1] = -1;
LEDMap[16][9][0] = -1;
LEDMap[16][9][1] = -1;
LEDMap[17][9][0] = -1;
LEDMap[17][9][1] = -1;
LEDMap[0][10][0] = 55;
LEDMap[0][10][1] = -1;
LEDMap[1][10][0] = -1;
LEDMap[1][10][1] = -1;
LEDMap[2][10][0] = -1;
LEDMap[2][10][1] = -1;
LEDMap[3][10][0] = -1;
LEDMap[3][10][1] = -1;
LEDMap[4][10][0] = 87;
LEDMap[4][10][1] = -1;
LEDMap[5][10][0] = -1;
LEDMap[5][10][1] = -1;
LEDMap[6][10][0] = -1;
LEDMap[6][10][1] = -1;
LEDMap[7][10][0] = 205;
LEDMap[7][10][1] = 206;
LEDMap[8][10][0] = -1;
LEDMap[8][10][1] = -1;
LEDMap[9][10][0] = -1;
LEDMap[9][10][1] = -1;
LEDMap[10][10][0] = -1;
LEDMap[10][10][1] = -1;
LEDMap[11][10][0] = 237;
LEDMap[11][10][1] = 238;
LEDMap[12][10][0] = -1;
LEDMap[12][10][1] = -1;
LEDMap[13][10][0] = -1;
LEDMap[13][10][1] = -1;
LEDMap[14][10][0] = -1;
LEDMap[14][10][1] = -1;
LEDMap[15][10][0] = 355;
LEDMap[15][10][1] = 356;
LEDMap[16][10][0] = -1;
LEDMap[16][10][1] = -1;
LEDMap[17][10][0] = -1;
LEDMap[17][10][1] = -1;
LEDMap[0][11][0] = 53;
LEDMap[0][11][1] = 54;
LEDMap[1][11][0] = -1;
LEDMap[1][11][1] = -1;
LEDMap[2][11][0] = -1;
LEDMap[2][11][1] = -1;
LEDMap[3][11][0] = -1;
LEDMap[3][11][1] = -1;
LEDMap[4][11][0] = 88;
LEDMap[4][11][1] = 89;
LEDMap[5][11][0] = -1;
LEDMap[5][11][1] = -1;
LEDMap[6][11][0] = -1;
LEDMap[6][11][1] = -1;
LEDMap[7][11][0] = -1;
LEDMap[7][11][1] = -1;
LEDMap[8][11][0] = 204;
LEDMap[8][11][1] = -1;
LEDMap[9][11][0] = -1;
LEDMap[9][11][1] = -1;
LEDMap[10][11][0] = -1;
LEDMap[10][11][1] = -1;
LEDMap[11][11][0] = 239;
LEDMap[11][11][1] = -1;
LEDMap[12][11][0] = -1;
LEDMap[12][11][1] = -1;
LEDMap[13][11][0] = -1;
LEDMap[13][11][1] = -1;
LEDMap[14][11][0] = -1;
LEDMap[14][11][1] = -1;
LEDMap[15][11][0] = 354;
LEDMap[15][11][1] = -1;
LEDMap[16][11][0] = -1;
LEDMap[16][11][1] = -1;
LEDMap[17][11][0] = -1;
LEDMap[17][11][1] = -1;
LEDMap[0][12][0] = 52;
LEDMap[0][12][1] = -1;
LEDMap[1][12][0] = -1;
LEDMap[1][12][1] = -1;
LEDMap[2][12][0] = -1;
LEDMap[2][12][1] = -1;
LEDMap[3][12][0] = -1;
LEDMap[3][12][1] = -1;
LEDMap[4][12][0] = 90;
LEDMap[4][12][1] = -1;
LEDMap[5][12][0] = -1;
LEDMap[5][12][1] = -1;
LEDMap[6][12][0] = -1;
LEDMap[6][12][1] = -1;
LEDMap[7][12][0] = -1;
LEDMap[7][12][1] = -1;
LEDMap[8][12][0] = 202;
LEDMap[8][12][1] = 203;
LEDMap[9][12][0] = -1;
LEDMap[9][12][1] = -1;
LEDMap[10][12][0] = -1;
LEDMap[10][12][1] = -1;
LEDMap[11][12][0] = 240;
LEDMap[11][12][1] = 241;
LEDMap[12][12][0] = -1;
LEDMap[12][12][1] = -1;
LEDMap[13][12][0] = -1;
LEDMap[13][12][1] = -1;
LEDMap[14][12][0] = -1;
LEDMap[14][12][1] = -1;
LEDMap[15][12][0] = 352;
LEDMap[15][12][1] = 353;
LEDMap[16][12][0] = -1;
LEDMap[16][12][1] = -1;
LEDMap[17][12][0] = -1;
LEDMap[17][12][1] = -1;
LEDMap[0][13][0] = 51;
LEDMap[0][13][1] = -1;
LEDMap[1][13][0] = -1;
LEDMap[1][13][1] = -1;
LEDMap[2][13][0] = -1;
LEDMap[2][13][1] = -1;
LEDMap[3][13][0] = -1;
LEDMap[3][13][1] = -1;
LEDMap[4][13][0] = 91;
LEDMap[4][13][1] = -1;
LEDMap[5][13][0] = -1;
LEDMap[5][13][1] = -1;
LEDMap[6][13][0] = -1;
LEDMap[6][13][1] = -1;
LEDMap[7][13][0] = -1;
LEDMap[7][13][1] = -1;
LEDMap[8][13][0] = 201;
LEDMap[8][13][1] = -1;
LEDMap[9][13][0] = -1;
LEDMap[9][13][1] = -1;
LEDMap[10][13][0] = -1;
LEDMap[10][13][1] = -1;
LEDMap[11][13][0] = 242;
LEDMap[11][13][1] = -1;
LEDMap[12][13][0] = -1;
LEDMap[12][13][1] = -1;
LEDMap[13][13][0] = -1;
LEDMap[13][13][1] = -1;
LEDMap[14][13][0] = -1;
LEDMap[14][13][1] = -1;
LEDMap[15][13][0] = 351;
LEDMap[15][13][1] = -1;
LEDMap[16][13][0] = -1;
LEDMap[16][13][1] = -1;
LEDMap[17][13][0] = -1;
LEDMap[17][13][1] = -1;
LEDMap[0][14][0] = 49;
LEDMap[0][14][1] = 50;
LEDMap[1][14][0] = -1;
LEDMap[1][14][1] = -1;
LEDMap[2][14][0] = -1;
LEDMap[2][14][1] = -1;
LEDMap[3][14][0] = -1;
LEDMap[3][14][1] = -1;
LEDMap[4][14][0] = 92;
LEDMap[4][14][1] = 93;
LEDMap[5][14][0] = -1;
LEDMap[5][14][1] = -1;
LEDMap[6][14][0] = -1;
LEDMap[6][14][1] = -1;
LEDMap[7][14][0] = -1;
LEDMap[7][14][1] = -1;
LEDMap[8][14][0] = 199;
LEDMap[8][14][1] = 200;
LEDMap[9][14][0] = -1;
LEDMap[9][14][1] = -1;
LEDMap[10][14][0] = -1;
LEDMap[10][14][1] = -1;
LEDMap[11][14][0] = 243;
LEDMap[11][14][1] = 244;
LEDMap[12][14][0] = -1;
LEDMap[12][14][1] = -1;
LEDMap[13][14][0] = -1;
LEDMap[13][14][1] = -1;
LEDMap[14][14][0] = -1;
LEDMap[14][14][1] = -1;
LEDMap[15][14][0] = 350;
LEDMap[15][14][1] = -1;
LEDMap[16][14][0] = -1;
LEDMap[16][14][1] = -1;
LEDMap[17][14][0] = -1;
LEDMap[17][14][1] = -1;
LEDMap[0][15][0] = 48;
LEDMap[0][15][1] = -1;
LEDMap[1][15][0] = -1;
LEDMap[1][15][1] = -1;
LEDMap[2][15][0] = -1;
LEDMap[2][15][1] = -1;
LEDMap[3][15][0] = -1;
LEDMap[3][15][1] = -1;
LEDMap[4][15][0] = 94;
LEDMap[4][15][1] = -1;
LEDMap[5][15][0] = -1;
LEDMap[5][15][1] = -1;
LEDMap[6][15][0] = -1;
LEDMap[6][15][1] = -1;
LEDMap[7][15][0] = -1;
LEDMap[7][15][1] = -1;
LEDMap[8][15][0] = 198;
LEDMap[8][15][1] = -1;
LEDMap[9][15][0] = -1;
LEDMap[9][15][1] = -1;
LEDMap[10][15][0] = -1;
LEDMap[10][15][1] = -1;
LEDMap[11][15][0] = 245;
LEDMap[11][15][1] = -1;
LEDMap[12][15][0] = -1;
LEDMap[12][15][1] = -1;
LEDMap[13][15][0] = -1;
LEDMap[13][15][1] = -1;
LEDMap[14][15][0] = -1;
LEDMap[14][15][1] = -1;
LEDMap[15][15][0] = 348;
LEDMap[15][15][1] = 349;
LEDMap[16][15][0] = -1;
LEDMap[16][15][1] = -1;
LEDMap[17][15][0] = -1;
LEDMap[17][15][1] = -1;
LEDMap[0][16][0] = 46;
LEDMap[0][16][1] = 47;
LEDMap[1][16][0] = -1;
LEDMap[1][16][1] = -1;
LEDMap[2][16][0] = -1;
LEDMap[2][16][1] = -1;
LEDMap[3][16][0] = -1;
LEDMap[3][16][1] = -1;
LEDMap[4][16][0] = 95;
LEDMap[4][16][1] = 96;
LEDMap[5][16][0] = -1;
LEDMap[5][16][1] = -1;
LEDMap[6][16][0] = -1;
LEDMap[6][16][1] = -1;
LEDMap[7][16][0] = -1;
LEDMap[7][16][1] = -1;
LEDMap[8][16][0] = 196;
LEDMap[8][16][1] = 197;
LEDMap[9][16][0] = -1;
LEDMap[9][16][1] = -1;
LEDMap[10][16][0] = -1;
LEDMap[10][16][1] = -1;
LEDMap[11][16][0] = 246;
LEDMap[11][16][1] = -1;
LEDMap[12][16][0] = -1;
LEDMap[12][16][1] = -1;
LEDMap[13][16][0] = -1;
LEDMap[13][16][1] = -1;
LEDMap[14][16][0] = -1;
LEDMap[14][16][1] = -1;
LEDMap[15][16][0] = 347;
LEDMap[15][16][1] = -1;
LEDMap[16][16][0] = -1;
LEDMap[16][16][1] = -1;
LEDMap[17][16][0] = -1;
LEDMap[17][16][1] = -1;
LEDMap[0][17][0] = 45;
LEDMap[0][17][1] = -1;
LEDMap[1][17][0] = -1;
LEDMap[1][17][1] = -1;
LEDMap[2][17][0] = -1;
LEDMap[2][17][1] = -1;
LEDMap[3][17][0] = -1;
LEDMap[3][17][1] = -1;
LEDMap[4][17][0] = 97;
LEDMap[4][17][1] = -1;
LEDMap[5][17][0] = -1;
LEDMap[5][17][1] = -1;
LEDMap[6][17][0] = -1;
LEDMap[6][17][1] = -1;
LEDMap[7][17][0] = -1;
LEDMap[7][17][1] = -1;
LEDMap[8][17][0] = 195;
LEDMap[8][17][1] = -1;
LEDMap[9][17][0] = -1;
LEDMap[9][17][1] = -1;
LEDMap[10][17][0] = -1;
LEDMap[10][17][1] = -1;
LEDMap[11][17][0] = 247;
LEDMap[11][17][1] = 248;
LEDMap[12][17][0] = -1;
LEDMap[12][17][1] = -1;
LEDMap[13][17][0] = -1;
LEDMap[13][17][1] = -1;
LEDMap[14][17][0] = -1;
LEDMap[14][17][1] = -1;
LEDMap[15][17][0] = 345;
LEDMap[15][17][1] = 346;
LEDMap[16][17][0] = -1;
LEDMap[16][17][1] = -1;
LEDMap[17][17][0] = -1;
LEDMap[17][17][1] = -1;
LEDMap[0][18][0] = 43;
LEDMap[0][18][1] = 44;
LEDMap[1][18][0] = -1;
LEDMap[1][18][1] = -1;
LEDMap[2][18][0] = -1;
LEDMap[2][18][1] = -1;
LEDMap[3][18][0] = -1;
LEDMap[3][18][1] = -1;
LEDMap[4][18][0] = 98;
LEDMap[4][18][1] = 99;
LEDMap[5][18][0] = -1;
LEDMap[5][18][1] = -1;
LEDMap[6][18][0] = -1;
LEDMap[6][18][1] = -1;
LEDMap[7][18][0] = -1;
LEDMap[7][18][1] = -1;
LEDMap[8][18][0] = 193;
LEDMap[8][18][1] = 194;
LEDMap[9][18][0] = -1;
LEDMap[9][18][1] = -1;
LEDMap[10][18][0] = -1;
LEDMap[10][18][1] = -1;
LEDMap[11][18][0] = 249;
LEDMap[11][18][1] = -1;
LEDMap[12][18][0] = -1;
LEDMap[12][18][1] = -1;
LEDMap[13][18][0] = -1;
LEDMap[13][18][1] = -1;
LEDMap[14][18][0] = -1;
LEDMap[14][18][1] = -1;
LEDMap[15][18][0] = 344;
LEDMap[15][18][1] = -1;
LEDMap[16][18][0] = -1;
LEDMap[16][18][1] = -1;
LEDMap[17][18][0] = -1;
LEDMap[17][18][1] = -1;
LEDMap[0][19][0] = 42;
LEDMap[0][19][1] = -1;
LEDMap[1][19][0] = -1;
LEDMap[1][19][1] = -1;
LEDMap[2][19][0] = -1;
LEDMap[2][19][1] = -1;
LEDMap[3][19][0] = -1;
LEDMap[3][19][1] = -1;
LEDMap[4][19][0] = 100;
LEDMap[4][19][1] = -1;
LEDMap[5][19][0] = -1;
LEDMap[5][19][1] = -1;
LEDMap[6][19][0] = -1;
LEDMap[6][19][1] = -1;
LEDMap[7][19][0] = -1;
LEDMap[7][19][1] = -1;
LEDMap[8][19][0] = 192;
LEDMap[8][19][1] = -1;
LEDMap[9][19][0] = -1;
LEDMap[9][19][1] = -1;
LEDMap[10][19][0] = -1;
LEDMap[10][19][1] = -1;
LEDMap[11][19][0] = 250;
LEDMap[11][19][1] = 251;
LEDMap[12][19][0] = -1;
LEDMap[12][19][1] = -1;
LEDMap[13][19][0] = -1;
LEDMap[13][19][1] = -1;
LEDMap[14][19][0] = -1;
LEDMap[14][19][1] = -1;
LEDMap[15][19][0] = 342;
LEDMap[15][19][1] = 343;
LEDMap[16][19][0] = -1;
LEDMap[16][19][1] = -1;
LEDMap[17][19][0] = -1;
LEDMap[17][19][1] = -1;
LEDMap[0][20][0] = 40;
LEDMap[0][20][1] = 41;
LEDMap[1][20][0] = -1;
LEDMap[1][20][1] = -1;
LEDMap[2][20][0] = -1;
LEDMap[2][20][1] = -1;
LEDMap[3][20][0] = -1;
LEDMap[3][20][1] = -1;
LEDMap[4][20][0] = 101;
LEDMap[4][20][1] = 102;
LEDMap[5][20][0] = -1;
LEDMap[5][20][1] = -1;
LEDMap[6][20][0] = -1;
LEDMap[6][20][1] = -1;
LEDMap[7][20][0] = -1;
LEDMap[7][20][1] = -1;
LEDMap[8][20][0] = 190;
LEDMap[8][20][1] = 191;
LEDMap[9][20][0] = -1;
LEDMap[9][20][1] = -1;
LEDMap[10][20][0] = -1;
LEDMap[10][20][1] = -1;
LEDMap[11][20][0] = 252;
LEDMap[11][20][1] = -1;
LEDMap[12][20][0] = -1;
LEDMap[12][20][1] = -1;
LEDMap[13][20][0] = -1;
LEDMap[13][20][1] = -1;
LEDMap[14][20][0] = -1;
LEDMap[14][20][1] = -1;
LEDMap[15][20][0] = 341;
LEDMap[15][20][1] = -1;
LEDMap[16][20][0] = -1;
LEDMap[16][20][1] = -1;
LEDMap[17][20][0] = -1;
LEDMap[17][20][1] = -1;
LEDMap[0][21][0] = 39;
LEDMap[0][21][1] = -1;
LEDMap[1][21][0] = -1;
LEDMap[1][21][1] = -1;
LEDMap[2][21][0] = -1;
LEDMap[2][21][1] = -1;
LEDMap[3][21][0] = -1;
LEDMap[3][21][1] = -1;
LEDMap[4][21][0] = 103;
LEDMap[4][21][1] = -1;
LEDMap[5][21][0] = -1;
LEDMap[5][21][1] = -1;
LEDMap[6][21][0] = -1;
LEDMap[6][21][1] = -1;
LEDMap[7][21][0] = -1;
LEDMap[7][21][1] = -1;
LEDMap[8][21][0] = 189;
LEDMap[8][21][1] = -1;
LEDMap[9][21][0] = -1;
LEDMap[9][21][1] = -1;
LEDMap[10][21][0] = -1;
LEDMap[10][21][1] = -1;
LEDMap[11][21][0] = -1;
LEDMap[11][21][1] = -1;
LEDMap[12][21][0] = 253;
LEDMap[12][21][1] = 254;
LEDMap[13][21][0] = -1;
LEDMap[13][21][1] = -1;
LEDMap[14][21][0] = -1;
LEDMap[14][21][1] = -1;
LEDMap[15][21][0] = 339;
LEDMap[15][21][1] = 340;
LEDMap[16][21][0] = -1;
LEDMap[16][21][1] = -1;
LEDMap[17][21][0] = -1;
LEDMap[17][21][1] = -1;
LEDMap[0][22][0] = 37;
LEDMap[0][22][1] = 38;
LEDMap[1][22][0] = -1;
LEDMap[1][22][1] = -1;
LEDMap[2][22][0] = -1;
LEDMap[2][22][1] = -1;
LEDMap[3][22][0] = -1;
LEDMap[3][22][1] = -1;
LEDMap[4][22][0] = 104;
LEDMap[4][22][1] = 105;
LEDMap[5][22][0] = -1;
LEDMap[5][22][1] = -1;
LEDMap[6][22][0] = -1;
LEDMap[6][22][1] = -1;
LEDMap[7][22][0] = -1;
LEDMap[7][22][1] = -1;
LEDMap[8][22][0] = 187;
LEDMap[8][22][1] = 188;
LEDMap[9][22][0] = -1;
LEDMap[9][22][1] = -1;
LEDMap[10][22][0] = -1;
LEDMap[10][22][1] = -1;
LEDMap[11][22][0] = -1;
LEDMap[11][22][1] = -1;
LEDMap[12][22][0] = 255;
LEDMap[12][22][1] = -1;
LEDMap[13][22][0] = -1;
LEDMap[13][22][1] = -1;
LEDMap[14][22][0] = -1;
LEDMap[14][22][1] = -1;
LEDMap[15][22][0] = 338;
LEDMap[15][22][1] = -1;
LEDMap[16][22][0] = -1;
LEDMap[16][22][1] = -1;
LEDMap[17][22][0] = -1;
LEDMap[17][22][1] = -1;
LEDMap[0][23][0] = 36;
LEDMap[0][23][1] = -1;
LEDMap[1][23][0] = -1;
LEDMap[1][23][1] = -1;
LEDMap[2][23][0] = -1;
LEDMap[2][23][1] = -1;
LEDMap[3][23][0] = -1;
LEDMap[3][23][1] = -1;
LEDMap[4][23][0] = 106;
LEDMap[4][23][1] = -1;
LEDMap[5][23][0] = -1;
LEDMap[5][23][1] = -1;
LEDMap[6][23][0] = -1;
LEDMap[6][23][1] = -1;
LEDMap[7][23][0] = -1;
LEDMap[7][23][1] = -1;
LEDMap[8][23][0] = 186;
LEDMap[8][23][1] = -1;
LEDMap[9][23][0] = -1;
LEDMap[9][23][1] = -1;
LEDMap[10][23][0] = -1;
LEDMap[10][23][1] = -1;
LEDMap[11][23][0] = -1;
LEDMap[11][23][1] = -1;
LEDMap[12][23][0] = 256;
LEDMap[12][23][1] = 257;
LEDMap[13][23][0] = -1;
LEDMap[13][23][1] = -1;
LEDMap[14][23][0] = -1;
LEDMap[14][23][1] = -1;
LEDMap[15][23][0] = 336;
LEDMap[15][23][1] = 337;
LEDMap[16][23][0] = -1;
LEDMap[16][23][1] = -1;
LEDMap[17][23][0] = -1;
LEDMap[17][23][1] = -1;
LEDMap[0][24][0] = 34;
LEDMap[0][24][1] = 35;
LEDMap[1][24][0] = -1;
LEDMap[1][24][1] = -1;
LEDMap[2][24][0] = -1;
LEDMap[2][24][1] = -1;
LEDMap[3][24][0] = -1;
LEDMap[3][24][1] = -1;
LEDMap[4][24][0] = 107;
LEDMap[4][24][1] = 108;
LEDMap[5][24][0] = -1;
LEDMap[5][24][1] = -1;
LEDMap[6][24][0] = -1;
LEDMap[6][24][1] = -1;
LEDMap[7][24][0] = -1;
LEDMap[7][24][1] = -1;
LEDMap[8][24][0] = 184;
LEDMap[8][24][1] = 185;
LEDMap[9][24][0] = -1;
LEDMap[9][24][1] = -1;
LEDMap[10][24][0] = -1;
LEDMap[10][24][1] = -1;
LEDMap[11][24][0] = -1;
LEDMap[11][24][1] = -1;
LEDMap[12][24][0] = 258;
LEDMap[12][24][1] = -1;
LEDMap[13][24][0] = -1;
LEDMap[13][24][1] = -1;
LEDMap[14][24][0] = -1;
LEDMap[14][24][1] = -1;
LEDMap[15][24][0] = 335;
LEDMap[15][24][1] = -1;
LEDMap[16][24][0] = -1;
LEDMap[16][24][1] = -1;
LEDMap[17][24][0] = -1;
LEDMap[17][24][1] = -1;
LEDMap[0][25][0] = 33;
LEDMap[0][25][1] = -1;
LEDMap[1][25][0] = -1;
LEDMap[1][25][1] = -1;
LEDMap[2][25][0] = -1;
LEDMap[2][25][1] = -1;
LEDMap[3][25][0] = -1;
LEDMap[3][25][1] = -1;
LEDMap[4][25][0] = 109;
LEDMap[4][25][1] = -1;
LEDMap[5][25][0] = -1;
LEDMap[5][25][1] = -1;
LEDMap[6][25][0] = -1;
LEDMap[6][25][1] = -1;
LEDMap[7][25][0] = -1;
LEDMap[7][25][1] = -1;
LEDMap[8][25][0] = 183;
LEDMap[8][25][1] = -1;
LEDMap[9][25][0] = -1;
LEDMap[9][25][1] = -1;
LEDMap[10][25][0] = -1;
LEDMap[10][25][1] = -1;
LEDMap[11][25][0] = -1;
LEDMap[11][25][1] = -1;
LEDMap[12][25][0] = 259;
LEDMap[12][25][1] = 260;
LEDMap[13][25][0] = -1;
LEDMap[13][25][1] = -1;
LEDMap[14][25][0] = -1;
LEDMap[14][25][1] = -1;
LEDMap[15][25][0] = 333;
LEDMap[15][25][1] = 334;
LEDMap[16][25][0] = -1;
LEDMap[16][25][1] = -1;
LEDMap[17][25][0] = -1;
LEDMap[17][25][1] = -1;
LEDMap[0][26][0] = 31;
LEDMap[0][26][1] = 32;
LEDMap[1][26][0] = -1;
LEDMap[1][26][1] = -1;
LEDMap[2][26][0] = -1;
LEDMap[2][26][1] = -1;
LEDMap[3][26][0] = -1;
LEDMap[3][26][1] = -1;
LEDMap[4][26][0] = 110;
LEDMap[4][26][1] = 111;
LEDMap[5][26][0] = -1;
LEDMap[5][26][1] = -1;
LEDMap[6][26][0] = -1;
LEDMap[6][26][1] = -1;
LEDMap[7][26][0] = -1;
LEDMap[7][26][1] = -1;
LEDMap[8][26][0] = 181;
LEDMap[8][26][1] = 182;
LEDMap[9][26][0] = -1;
LEDMap[9][26][1] = -1;
LEDMap[10][26][0] = -1;
LEDMap[10][26][1] = -1;
LEDMap[11][26][0] = -1;
LEDMap[11][26][1] = -1;
LEDMap[12][26][0] = 261;
LEDMap[12][26][1] = -1;
LEDMap[13][26][0] = -1;
LEDMap[13][26][1] = -1;
LEDMap[14][26][0] = -1;
LEDMap[14][26][1] = -1;
LEDMap[15][26][0] = 332;
LEDMap[15][26][1] = -1;
LEDMap[16][26][0] = -1;
LEDMap[16][26][1] = -1;
LEDMap[17][26][0] = -1;
LEDMap[17][26][1] = -1;
LEDMap[0][27][0] = 30;
LEDMap[0][27][1] = -1;
LEDMap[1][27][0] = -1;
LEDMap[1][27][1] = -1;
LEDMap[2][27][0] = -1;
LEDMap[2][27][1] = -1;
LEDMap[3][27][0] = -1;
LEDMap[3][27][1] = -1;
LEDMap[4][27][0] = 112;
LEDMap[4][27][1] = -1;
LEDMap[5][27][0] = -1;
LEDMap[5][27][1] = -1;
LEDMap[6][27][0] = -1;
LEDMap[6][27][1] = -1;
LEDMap[7][27][0] = -1;
LEDMap[7][27][1] = -1;
LEDMap[8][27][0] = 180;
LEDMap[8][27][1] = -1;
LEDMap[9][27][0] = -1;
LEDMap[9][27][1] = -1;
LEDMap[10][27][0] = -1;
LEDMap[10][27][1] = -1;
LEDMap[11][27][0] = -1;
LEDMap[11][27][1] = -1;
LEDMap[12][27][0] = 262;
LEDMap[12][27][1] = 263;
LEDMap[13][27][0] = -1;
LEDMap[13][27][1] = -1;
LEDMap[14][27][0] = -1;
LEDMap[14][27][1] = -1;
LEDMap[15][27][0] = -1;
LEDMap[15][27][1] = -1;
LEDMap[16][27][0] = 331;
LEDMap[16][27][1] = -1;
LEDMap[17][27][0] = -1;
LEDMap[17][27][1] = -1;
LEDMap[0][28][0] = 29;
LEDMap[0][28][1] = -1;
LEDMap[1][28][0] = -1;
LEDMap[1][28][1] = -1;
LEDMap[2][28][0] = -1;
LEDMap[2][28][1] = -1;
LEDMap[3][28][0] = -1;
LEDMap[3][28][1] = -1;
LEDMap[4][28][0] = 113;
LEDMap[4][28][1] = 114;
LEDMap[5][28][0] = -1;
LEDMap[5][28][1] = -1;
LEDMap[6][28][0] = -1;
LEDMap[6][28][1] = -1;
LEDMap[7][28][0] = 0;
LEDMap[7][28][1] = -1;
LEDMap[8][28][0] = 178;
LEDMap[8][28][1] = 179;
LEDMap[9][28][0] = -1;
LEDMap[9][28][1] = -1;
LEDMap[10][28][0] = -1;
LEDMap[10][28][1] = -1;
LEDMap[11][28][0] = -1;
LEDMap[11][28][1] = -1;
LEDMap[12][28][0] = 264;
LEDMap[12][28][1] = -1;
LEDMap[13][28][0] = -1;
LEDMap[13][28][1] = -1;
LEDMap[14][28][0] = -1;
LEDMap[14][28][1] = -1;
LEDMap[15][28][0] = 329;
LEDMap[15][28][1] = -1;
LEDMap[16][28][0] = -1;
LEDMap[16][28][1] = -1;
LEDMap[17][28][0] = -1;
LEDMap[17][28][1] = -1;
LEDMap[0][29][0] = 27;
LEDMap[0][29][1] = 28;
LEDMap[1][29][0] = -1;
LEDMap[1][29][1] = -1;
LEDMap[2][29][0] = -1;
LEDMap[2][29][1] = -1;
LEDMap[3][29][0] = -1;
LEDMap[3][29][1] = -1;
LEDMap[4][29][0] = 115;
LEDMap[4][29][1] = -1;
LEDMap[5][29][0] = -1;
LEDMap[5][29][1] = -1;
LEDMap[6][29][0] = -1;
LEDMap[6][29][1] = -1;
LEDMap[7][29][0] = -1;
LEDMap[7][29][1] = -1;
LEDMap[8][29][0] = 177;
LEDMap[8][29][1] = -1;
LEDMap[9][29][0] = -1;
LEDMap[9][29][1] = -1;
LEDMap[10][29][0] = -1;
LEDMap[10][29][1] = -1;
LEDMap[11][29][0] = -1;
LEDMap[11][29][1] = -1;
LEDMap[12][29][0] = 265;
LEDMap[12][29][1] = 266;
LEDMap[13][29][0] = -1;
LEDMap[13][29][1] = -1;
LEDMap[14][29][0] = -1;
LEDMap[14][29][1] = -1;
LEDMap[15][29][0] = 327;
LEDMap[15][29][1] = 328;
LEDMap[16][29][0] = -1;
LEDMap[16][29][1] = -1;
LEDMap[17][29][0] = -1;
LEDMap[17][29][1] = -1;
LEDMap[0][30][0] = 26;
LEDMap[0][30][1] = -1;
LEDMap[1][30][0] = -1;
LEDMap[1][30][1] = -1;
LEDMap[2][30][0] = -1;
LEDMap[2][30][1] = -1;
LEDMap[3][30][0] = -1;
LEDMap[3][30][1] = -1;
LEDMap[4][30][0] = 116;
LEDMap[4][30][1] = 117;
LEDMap[5][30][0] = -1;
LEDMap[5][30][1] = -1;
LEDMap[6][30][0] = -1;
LEDMap[6][30][1] = -1;
LEDMap[7][30][0] = -1;
LEDMap[7][30][1] = -1;
LEDMap[8][30][0] = 175;
LEDMap[8][30][1] = 176;
LEDMap[9][30][0] = -1;
LEDMap[9][30][1] = -1;
LEDMap[10][30][0] = -1;
LEDMap[10][30][1] = -1;
LEDMap[11][30][0] = -1;
LEDMap[11][30][1] = -1;
LEDMap[12][30][0] = 267;
LEDMap[12][30][1] = -1;
LEDMap[13][30][0] = -1;
LEDMap[13][30][1] = -1;
LEDMap[14][30][0] = -1;
LEDMap[14][30][1] = -1;
LEDMap[15][30][0] = 326;
LEDMap[15][30][1] = -1;
LEDMap[16][30][0] = -1;
LEDMap[16][30][1] = -1;
LEDMap[17][30][0] = -1;
LEDMap[17][30][1] = -1;
LEDMap[0][31][0] = 25;
LEDMap[0][31][1] = -1;
LEDMap[1][31][0] = 24;
LEDMap[1][31][1] = -1;
LEDMap[2][31][0] = -1;
LEDMap[2][31][1] = -1;
LEDMap[3][31][0] = -1;
LEDMap[3][31][1] = -1;
LEDMap[4][31][0] = 118;
LEDMap[4][31][1] = -1;
LEDMap[5][31][0] = -1;
LEDMap[5][31][1] = -1;
LEDMap[6][31][0] = -1;
LEDMap[6][31][1] = -1;
LEDMap[7][31][0] = -1;
LEDMap[7][31][1] = -1;
LEDMap[8][31][0] = 174;
LEDMap[8][31][1] = -1;
LEDMap[9][31][0] = -1;
LEDMap[9][31][1] = -1;
LEDMap[10][31][0] = -1;
LEDMap[10][31][1] = -1;
LEDMap[11][31][0] = -1;
LEDMap[11][31][1] = -1;
LEDMap[12][31][0] = 268;
LEDMap[12][31][1] = 269;
LEDMap[13][31][0] = -1;
LEDMap[13][31][1] = -1;
LEDMap[14][31][0] = -1;
LEDMap[14][31][1] = -1;
LEDMap[15][31][0] = 324;
LEDMap[15][31][1] = 325;
LEDMap[16][31][0] = -1;
LEDMap[16][31][1] = -1;
LEDMap[17][31][0] = -1;
LEDMap[17][31][1] = -1;
LEDMap[0][32][0] = -1;
LEDMap[0][32][1] = -1;
LEDMap[1][32][0] = 23;
LEDMap[1][32][1] = -1;
LEDMap[2][32][0] = -1;
LEDMap[2][32][1] = -1;
LEDMap[3][32][0] = -1;
LEDMap[3][32][1] = -1;
LEDMap[4][32][0] = 119;
LEDMap[4][32][1] = 120;
LEDMap[5][32][0] = -1;
LEDMap[5][32][1] = -1;
LEDMap[6][32][0] = -1;
LEDMap[6][32][1] = -1;
LEDMap[7][32][0] = -1;
LEDMap[7][32][1] = -1;
LEDMap[8][32][0] = 172;
LEDMap[8][32][1] = 173;
LEDMap[9][32][0] = -1;
LEDMap[9][32][1] = -1;
LEDMap[10][32][0] = -1;
LEDMap[10][32][1] = -1;
LEDMap[11][32][0] = -1;
LEDMap[11][32][1] = -1;
LEDMap[12][32][0] = 270;
LEDMap[12][32][1] = -1;
LEDMap[13][32][0] = -1;
LEDMap[13][32][1] = -1;
LEDMap[14][32][0] = -1;
LEDMap[14][32][1] = -1;
LEDMap[15][32][0] = 323;
LEDMap[15][32][1] = -1;
LEDMap[16][32][0] = -1;
LEDMap[16][32][1] = -1;
LEDMap[17][32][0] = -1;
LEDMap[17][32][1] = -1;
LEDMap[0][33][0] = -1;
LEDMap[0][33][1] = -1;
LEDMap[1][33][0] = 21;
LEDMap[1][33][1] = 22;
LEDMap[2][33][0] = -1;
LEDMap[2][33][1] = -1;
LEDMap[3][33][0] = -1;
LEDMap[3][33][1] = -1;
LEDMap[4][33][0] = 121;
LEDMap[4][33][1] = -1;
LEDMap[5][33][0] = -1;
LEDMap[5][33][1] = -1;
LEDMap[6][33][0] = -1;
LEDMap[6][33][1] = -1;
LEDMap[7][33][0] = -1;
LEDMap[7][33][1] = -1;
LEDMap[8][33][0] = 171;
LEDMap[8][33][1] = -1;
LEDMap[9][33][0] = -1;
LEDMap[9][33][1] = -1;
LEDMap[10][33][0] = -1;
LEDMap[10][33][1] = -1;
LEDMap[11][33][0] = -1;
LEDMap[11][33][1] = -1;
LEDMap[12][33][0] = 271;
LEDMap[12][33][1] = 272;
LEDMap[13][33][0] = -1;
LEDMap[13][33][1] = -1;
LEDMap[14][33][0] = -1;
LEDMap[14][33][1] = -1;
LEDMap[15][33][0] = 321;
LEDMap[15][33][1] = 322;
LEDMap[16][33][0] = -1;
LEDMap[16][33][1] = -1;
LEDMap[17][33][0] = -1;
LEDMap[17][33][1] = -1;
LEDMap[0][34][0] = -1;
LEDMap[0][34][1] = -1;
LEDMap[1][34][0] = 20;
LEDMap[1][34][1] = -1;
LEDMap[2][34][0] = -1;
LEDMap[2][34][1] = -1;
LEDMap[3][34][0] = -1;
LEDMap[3][34][1] = -1;
LEDMap[4][34][0] = 122;
LEDMap[4][34][1] = 123;
LEDMap[5][34][0] = -1;
LEDMap[5][34][1] = -1;
LEDMap[6][34][0] = -1;
LEDMap[6][34][1] = -1;
LEDMap[7][34][0] = -1;
LEDMap[7][34][1] = -1;
LEDMap[8][34][0] = 169;
LEDMap[8][34][1] = 170;
LEDMap[9][34][0] = -1;
LEDMap[9][34][1] = -1;
LEDMap[10][34][0] = -1;
LEDMap[10][34][1] = -1;
LEDMap[11][34][0] = -1;
LEDMap[11][34][1] = -1;
LEDMap[12][34][0] = 273;
LEDMap[12][34][1] = 274;
LEDMap[13][34][0] = -1;
LEDMap[13][34][1] = -1;
LEDMap[14][34][0] = -1;
LEDMap[14][34][1] = -1;
LEDMap[15][34][0] = 319;
LEDMap[15][34][1] = 320;
LEDMap[16][34][0] = -1;
LEDMap[16][34][1] = -1;
LEDMap[17][34][0] = -1;
LEDMap[17][34][1] = -1;
LEDMap[0][35][0] = -1;
LEDMap[0][35][1] = -1;
LEDMap[1][35][0] = 18;
LEDMap[1][35][1] = 19;
LEDMap[2][35][0] = -1;
LEDMap[2][35][1] = -1;
LEDMap[3][35][0] = -1;
LEDMap[3][35][1] = -1;
LEDMap[4][35][0] = 124;
LEDMap[4][35][1] = -1;
LEDMap[5][35][0] = -1;
LEDMap[5][35][1] = -1;
LEDMap[6][35][0] = -1;
LEDMap[6][35][1] = -1;
LEDMap[7][35][0] = -1;
LEDMap[7][35][1] = -1;
LEDMap[8][35][0] = 168;
LEDMap[8][35][1] = -1;
LEDMap[9][35][0] = -1;
LEDMap[9][35][1] = -1;
LEDMap[10][35][0] = -1;
LEDMap[10][35][1] = -1;
LEDMap[11][35][0] = -1;
LEDMap[11][35][1] = -1;
LEDMap[12][35][0] = 275;
LEDMap[12][35][1] = -1;
LEDMap[13][35][0] = -1;
LEDMap[13][35][1] = -1;
LEDMap[14][35][0] = -1;
LEDMap[14][35][1] = -1;
LEDMap[15][35][0] = 318;
LEDMap[15][35][1] = -1;
LEDMap[16][35][0] = -1;
LEDMap[16][35][1] = -1;
LEDMap[17][35][0] = -1;
LEDMap[17][35][1] = -1;
LEDMap[0][36][0] = -1;
LEDMap[0][36][1] = -1;
LEDMap[1][36][0] = 16;
LEDMap[1][36][1] = 17;
LEDMap[2][36][0] = -1;
LEDMap[2][36][1] = -1;
LEDMap[3][36][0] = -1;
LEDMap[3][36][1] = -1;
LEDMap[4][36][0] = 125;
LEDMap[4][36][1] = 126;
LEDMap[5][36][0] = -1;
LEDMap[5][36][1] = -1;
LEDMap[6][36][0] = -1;
LEDMap[6][36][1] = -1;
LEDMap[7][36][0] = -1;
LEDMap[7][36][1] = -1;
LEDMap[8][36][0] = 166;
LEDMap[8][36][1] = 167;
LEDMap[9][36][0] = -1;
LEDMap[9][36][1] = -1;
LEDMap[10][36][0] = -1;
LEDMap[10][36][1] = -1;
LEDMap[11][36][0] = -1;
LEDMap[11][36][1] = -1;
LEDMap[12][36][0] = 276;
LEDMap[12][36][1] = 277;
LEDMap[13][36][0] = -1;
LEDMap[13][36][1] = -1;
LEDMap[14][36][0] = -1;
LEDMap[14][36][1] = -1;
LEDMap[15][36][0] = 316;
LEDMap[15][36][1] = 317;
LEDMap[16][36][0] = -1;
LEDMap[16][36][1] = -1;
LEDMap[17][36][0] = -1;
LEDMap[17][36][1] = -1;
LEDMap[0][37][0] = -1;
LEDMap[0][37][1] = -1;
LEDMap[1][37][0] = 15;
LEDMap[1][37][1] = -1;
LEDMap[2][37][0] = -1;
LEDMap[2][37][1] = -1;
LEDMap[3][37][0] = -1;
LEDMap[3][37][1] = -1;
LEDMap[4][37][0] = 127;
LEDMap[4][37][1] = -1;
LEDMap[5][37][0] = -1;
LEDMap[5][37][1] = -1;
LEDMap[6][37][0] = -1;
LEDMap[6][37][1] = -1;
LEDMap[7][37][0] = -1;
LEDMap[7][37][1] = -1;
LEDMap[8][37][0] = 165;
LEDMap[8][37][1] = -1;
LEDMap[9][37][0] = -1;
LEDMap[9][37][1] = -1;
LEDMap[10][37][0] = -1;
LEDMap[10][37][1] = -1;
LEDMap[11][37][0] = -1;
LEDMap[11][37][1] = -1;
LEDMap[12][37][0] = 278;
LEDMap[12][37][1] = -1;
LEDMap[13][37][0] = -1;
LEDMap[13][37][1] = -1;
LEDMap[14][37][0] = -1;
LEDMap[14][37][1] = -1;
LEDMap[15][37][0] = 315;
LEDMap[15][37][1] = -1;
LEDMap[16][37][0] = -1;
LEDMap[16][37][1] = -1;
LEDMap[17][37][0] = -1;
LEDMap[17][37][1] = -1;
LEDMap[0][38][0] = -1;
LEDMap[0][38][1] = -1;
LEDMap[1][38][0] = 14;
LEDMap[1][38][1] = -1;
LEDMap[2][38][0] = -1;
LEDMap[2][38][1] = -1;
LEDMap[3][38][0] = -1;
LEDMap[3][38][1] = -1;
LEDMap[4][38][0] = 128;
LEDMap[4][38][1] = 129;
LEDMap[5][38][0] = -1;
LEDMap[5][38][1] = -1;
LEDMap[6][38][0] = -1;
LEDMap[6][38][1] = -1;
LEDMap[7][38][0] = -1;
LEDMap[7][38][1] = -1;
LEDMap[8][38][0] = 163;
LEDMap[8][38][1] = 164;
LEDMap[9][38][0] = -1;
LEDMap[9][38][1] = -1;
LEDMap[10][38][0] = -1;
LEDMap[10][38][1] = -1;
LEDMap[11][38][0] = -1;
LEDMap[11][38][1] = -1;
LEDMap[12][38][0] = 279;
LEDMap[12][38][1] = 280;
LEDMap[13][38][0] = -1;
LEDMap[13][38][1] = -1;
LEDMap[14][38][0] = -1;
LEDMap[14][38][1] = -1;
LEDMap[15][38][0] = 313;
LEDMap[15][38][1] = 314;
LEDMap[16][38][0] = -1;
LEDMap[16][38][1] = -1;
LEDMap[17][38][0] = -1;
LEDMap[17][38][1] = -1;
LEDMap[0][39][0] = -1;
LEDMap[0][39][1] = -1;
LEDMap[1][39][0] = 12;
LEDMap[1][39][1] = 13;
LEDMap[2][39][0] = -1;
LEDMap[2][39][1] = -1;
LEDMap[3][39][0] = -1;
LEDMap[3][39][1] = -1;
LEDMap[4][39][0] = 130;
LEDMap[4][39][1] = -1;
LEDMap[5][39][0] = -1;
LEDMap[5][39][1] = -1;
LEDMap[6][39][0] = -1;
LEDMap[6][39][1] = -1;
LEDMap[7][39][0] = -1;
LEDMap[7][39][1] = -1;
LEDMap[8][39][0] = 162;
LEDMap[8][39][1] = -1;
LEDMap[9][39][0] = -1;
LEDMap[9][39][1] = -1;
LEDMap[10][39][0] = -1;
LEDMap[10][39][1] = -1;
LEDMap[11][39][0] = -1;
LEDMap[11][39][1] = -1;
LEDMap[12][39][0] = 281;
LEDMap[12][39][1] = -1;
LEDMap[13][39][0] = -1;
LEDMap[13][39][1] = -1;
LEDMap[14][39][0] = -1;
LEDMap[14][39][1] = -1;
LEDMap[15][39][0] = 312;
LEDMap[15][39][1] = -1;
LEDMap[16][39][0] = -1;
LEDMap[16][39][1] = -1;
LEDMap[17][39][0] = -1;
LEDMap[17][39][1] = -1;
LEDMap[0][40][0] = -1;
LEDMap[0][40][1] = -1;
LEDMap[1][40][0] = 11;
LEDMap[1][40][1] = -1;
LEDMap[2][40][0] = -1;
LEDMap[2][40][1] = -1;
LEDMap[3][40][0] = -1;
LEDMap[3][40][1] = -1;
LEDMap[4][40][0] = 131;
LEDMap[4][40][1] = 132;
LEDMap[5][40][0] = -1;
LEDMap[5][40][1] = -1;
LEDMap[6][40][0] = -1;
LEDMap[6][40][1] = -1;
LEDMap[7][40][0] = -1;
LEDMap[7][40][1] = -1;
LEDMap[8][40][0] = 160;
LEDMap[8][40][1] = 161;
LEDMap[9][40][0] = -1;
LEDMap[9][40][1] = -1;
LEDMap[10][40][0] = -1;
LEDMap[10][40][1] = -1;
LEDMap[11][40][0] = -1;
LEDMap[11][40][1] = -1;
LEDMap[12][40][0] = 282;
LEDMap[12][40][1] = 283;
LEDMap[13][40][0] = -1;
LEDMap[13][40][1] = -1;
LEDMap[14][40][0] = -1;
LEDMap[14][40][1] = -1;
LEDMap[15][40][0] = 310;
LEDMap[15][40][1] = 311;
LEDMap[16][40][0] = -1;
LEDMap[16][40][1] = -1;
LEDMap[17][40][0] = -1;
LEDMap[17][40][1] = -1;
LEDMap[0][41][0] = -1;
LEDMap[0][41][1] = -1;
LEDMap[1][41][0] = 9;
LEDMap[1][41][1] = 10;
LEDMap[2][41][0] = -1;
LEDMap[2][41][1] = -1;
LEDMap[3][41][0] = -1;
LEDMap[3][41][1] = -1;
LEDMap[4][41][0] = 133;
LEDMap[4][41][1] = 134;
LEDMap[5][41][0] = -1;
LEDMap[5][41][1] = -1;
LEDMap[6][41][0] = -1;
LEDMap[6][41][1] = -1;
LEDMap[7][41][0] = -1;
LEDMap[7][41][1] = -1;
LEDMap[8][41][0] = 158;
LEDMap[8][41][1] = 159;
LEDMap[9][41][0] = -1;
LEDMap[9][41][1] = -1;
LEDMap[10][41][0] = -1;
LEDMap[10][41][1] = -1;
LEDMap[11][41][0] = -1;
LEDMap[11][41][1] = -1;
LEDMap[12][41][0] = 284;
LEDMap[12][41][1] = 285;
LEDMap[13][41][0] = -1;
LEDMap[13][41][1] = -1;
LEDMap[14][41][0] = -1;
LEDMap[14][41][1] = -1;
LEDMap[15][41][0] = 309;
LEDMap[15][41][1] = -1;
LEDMap[16][41][0] = -1;
LEDMap[16][41][1] = -1;
LEDMap[17][41][0] = -1;
LEDMap[17][41][1] = -1;
LEDMap[0][42][0] = -1;
LEDMap[0][42][1] = -1;
LEDMap[1][42][0] = 7;
LEDMap[1][42][1] = 8;
LEDMap[2][42][0] = -1;
LEDMap[2][42][1] = -1;
LEDMap[3][42][0] = -1;
LEDMap[3][42][1] = -1;
LEDMap[4][42][0] = 135;
LEDMap[4][42][1] = -1;
LEDMap[5][42][0] = -1;
LEDMap[5][42][1] = -1;
LEDMap[6][42][0] = -1;
LEDMap[6][42][1] = -1;
LEDMap[7][42][0] = -1;
LEDMap[7][42][1] = -1;
LEDMap[8][42][0] = 157;
LEDMap[8][42][1] = -1;
LEDMap[9][42][0] = -1;
LEDMap[9][42][1] = -1;
LEDMap[10][42][0] = -1;
LEDMap[10][42][1] = -1;
LEDMap[11][42][0] = -1;
LEDMap[11][42][1] = -1;
LEDMap[12][42][0] = 286;
LEDMap[12][42][1] = -1;
LEDMap[13][42][0] = -1;
LEDMap[13][42][1] = -1;
LEDMap[14][42][0] = -1;
LEDMap[14][42][1] = -1;
LEDMap[15][42][0] = 307;
LEDMap[15][42][1] = 308;
LEDMap[16][42][0] = -1;
LEDMap[16][42][1] = -1;
LEDMap[17][42][0] = -1;
LEDMap[17][42][1] = -1;
LEDMap[0][43][0] = -1;
LEDMap[0][43][1] = -1;
LEDMap[1][43][0] = 6;
LEDMap[1][43][1] = -1;
LEDMap[2][43][0] = -1;
LEDMap[2][43][1] = -1;
LEDMap[3][43][0] = -1;
LEDMap[3][43][1] = -1;
LEDMap[4][43][0] = 136;
LEDMap[4][43][1] = 137;
LEDMap[5][43][0] = -1;
LEDMap[5][43][1] = -1;
LEDMap[6][43][0] = -1;
LEDMap[6][43][1] = -1;
LEDMap[7][43][0] = -1;
LEDMap[7][43][1] = -1;
LEDMap[8][43][0] = 156;
LEDMap[8][43][1] = -1;
LEDMap[9][43][0] = -1;
LEDMap[9][43][1] = -1;
LEDMap[10][43][0] = -1;
LEDMap[10][43][1] = -1;
LEDMap[11][43][0] = -1;
LEDMap[11][43][1] = -1;
LEDMap[12][43][0] = 287;
LEDMap[12][43][1] = 288;
LEDMap[13][43][0] = -1;
LEDMap[13][43][1] = -1;
LEDMap[14][43][0] = -1;
LEDMap[14][43][1] = -1;
LEDMap[15][43][0] = 305;
LEDMap[15][43][1] = 306;
LEDMap[16][43][0] = -1;
LEDMap[16][43][1] = -1;
LEDMap[17][43][0] = -1;
LEDMap[17][43][1] = -1;
LEDMap[0][44][0] = -1;
LEDMap[0][44][1] = -1;
LEDMap[1][44][0] = 4;
LEDMap[1][44][1] = 5;
LEDMap[2][44][0] = -1;
LEDMap[2][44][1] = -1;
LEDMap[3][44][0] = -1;
LEDMap[3][44][1] = -1;
LEDMap[4][44][0] = 138;
LEDMap[4][44][1] = -1;
LEDMap[5][44][0] = -1;
LEDMap[5][44][1] = -1;
LEDMap[6][44][0] = -1;
LEDMap[6][44][1] = -1;
LEDMap[7][44][0] = -1;
LEDMap[7][44][1] = -1;
LEDMap[8][44][0] = 154;
LEDMap[8][44][1] = 155;
LEDMap[9][44][0] = -1;
LEDMap[9][44][1] = -1;
LEDMap[10][44][0] = -1;
LEDMap[10][44][1] = -1;
LEDMap[11][44][0] = -1;
LEDMap[11][44][1] = -1;
LEDMap[12][44][0] = 289;
LEDMap[12][44][1] = -1;
LEDMap[13][44][0] = -1;
LEDMap[13][44][1] = -1;
LEDMap[14][44][0] = -1;
LEDMap[14][44][1] = -1;
LEDMap[15][44][0] = 304;
LEDMap[15][44][1] = -1;
LEDMap[16][44][0] = -1;
LEDMap[16][44][1] = -1;
LEDMap[17][44][0] = -1;
LEDMap[17][44][1] = -1;
LEDMap[0][45][0] = -1;
LEDMap[0][45][1] = -1;
LEDMap[1][45][0] = 3;
LEDMap[1][45][1] = -1;
LEDMap[2][45][0] = -1;
LEDMap[2][45][1] = -1;
LEDMap[3][45][0] = -1;
LEDMap[3][45][1] = -1;
LEDMap[4][45][0] = 139;
LEDMap[4][45][1] = 140;
LEDMap[5][45][0] = -1;
LEDMap[5][45][1] = -1;
LEDMap[6][45][0] = -1;
LEDMap[6][45][1] = -1;
LEDMap[7][45][0] = -1;
LEDMap[7][45][1] = -1;
LEDMap[8][45][0] = 152;
LEDMap[8][45][1] = 153;
LEDMap[9][45][0] = -1;
LEDMap[9][45][1] = -1;
LEDMap[10][45][0] = -1;
LEDMap[10][45][1] = -1;
LEDMap[11][45][0] = -1;
LEDMap[11][45][1] = -1;
LEDMap[12][45][0] = 290;
LEDMap[12][45][1] = 291;
LEDMap[13][45][0] = -1;
LEDMap[13][45][1] = -1;
LEDMap[14][45][0] = -1;
LEDMap[14][45][1] = -1;
LEDMap[15][45][0] = 302;
LEDMap[15][45][1] = 303;
LEDMap[16][45][0] = -1;
LEDMap[16][45][1] = -1;
LEDMap[17][45][0] = -1;
LEDMap[17][45][1] = -1;
LEDMap[0][46][0] = -1;
LEDMap[0][46][1] = -1;
LEDMap[1][46][0] = 1;
LEDMap[1][46][1] = 2;
LEDMap[2][46][0] = -1;
LEDMap[2][46][1] = -1;
LEDMap[3][46][0] = -1;
LEDMap[3][46][1] = -1;
LEDMap[4][46][0] = 141;
LEDMap[4][46][1] = -1;
LEDMap[5][46][0] = -1;
LEDMap[5][46][1] = -1;
LEDMap[6][46][0] = -1;
LEDMap[6][46][1] = -1;
LEDMap[7][46][0] = -1;
LEDMap[7][46][1] = -1;
LEDMap[8][46][0] = 151;
LEDMap[8][46][1] = -1;
LEDMap[9][46][0] = -1;
LEDMap[9][46][1] = -1;
LEDMap[10][46][0] = -1;
LEDMap[10][46][1] = -1;
LEDMap[11][46][0] = -1;
LEDMap[11][46][1] = -1;
LEDMap[12][46][0] = 292;
LEDMap[12][46][1] = 293;
LEDMap[13][46][0] = -1;
LEDMap[13][46][1] = -1;
LEDMap[14][46][0] = -1;
LEDMap[14][46][1] = -1;
LEDMap[15][46][0] = -1;
LEDMap[15][46][1] = -1;
LEDMap[16][46][0] = 301;
LEDMap[16][46][1] = -1;
LEDMap[17][46][0] = -1;
LEDMap[17][46][1] = -1;
LEDMap[0][47][0] = -1;
LEDMap[0][47][1] = -1;
LEDMap[1][47][0] = -1;
LEDMap[1][47][1] = -1;
LEDMap[2][47][0] = -1;
LEDMap[2][47][1] = -1;
LEDMap[3][47][0] = -1;
LEDMap[3][47][1] = -1;
LEDMap[4][47][0] = 142;
LEDMap[4][47][1] = -1;
LEDMap[5][47][0] = 143;
LEDMap[5][47][1] = -1;
LEDMap[6][47][0] = -1;
LEDMap[6][47][1] = -1;
LEDMap[7][47][0] = -1;
LEDMap[7][47][1] = -1;
LEDMap[8][47][0] = 149;
LEDMap[8][47][1] = 150;
LEDMap[9][47][0] = -1;
LEDMap[9][47][1] = -1;
LEDMap[10][47][0] = -1;
LEDMap[10][47][1] = -1;
LEDMap[11][47][0] = -1;
LEDMap[11][47][1] = -1;
LEDMap[12][47][0] = 294;
LEDMap[12][47][1] = -1;
LEDMap[13][47][0] = -1;
LEDMap[13][47][1] = -1;
LEDMap[14][47][0] = -1;
LEDMap[14][47][1] = -1;
LEDMap[15][47][0] = 299;
LEDMap[15][47][1] = 300;
LEDMap[16][47][0] = -1;
LEDMap[16][47][1] = -1;
LEDMap[17][47][0] = -1;
LEDMap[17][47][1] = -1;
LEDMap[0][48][0] = -1;
LEDMap[0][48][1] = -1;
LEDMap[1][48][0] = -1;
LEDMap[1][48][1] = -1;
LEDMap[2][48][0] = -1;
LEDMap[2][48][1] = -1;
LEDMap[3][48][0] = -1;
LEDMap[3][48][1] = -1;
LEDMap[4][48][0] = -1;
LEDMap[4][48][1] = -1;
LEDMap[5][48][0] = 144;
LEDMap[5][48][1] = -1;
LEDMap[6][48][0] = 145;
LEDMap[6][48][1] = -1;
LEDMap[7][48][0] = 146;
LEDMap[7][48][1] = -1;
LEDMap[8][48][0] = 148;
LEDMap[8][48][1] = -1;
LEDMap[9][48][0] = -1;
LEDMap[9][48][1] = -1;
LEDMap[10][48][0] = -1;
LEDMap[10][48][1] = -1;
LEDMap[11][48][0] = -1;
LEDMap[11][48][1] = -1;
LEDMap[12][48][0] = -1;
LEDMap[12][48][1] = -1;
LEDMap[13][48][0] = 295;
LEDMap[13][48][1] = 296;
LEDMap[14][48][0] = 297;
LEDMap[14][48][1] = -1;
LEDMap[15][48][0] = 298;
LEDMap[15][48][1] = -1;
LEDMap[16][48][0] = -1;
LEDMap[16][48][1] = -1;
LEDMap[17][48][0] = -1;
LEDMap[17][48][1] = -1;
}
