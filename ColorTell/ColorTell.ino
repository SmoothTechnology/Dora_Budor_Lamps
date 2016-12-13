#include <FastLED.h>

#define LED_PIN  6

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define BRIGHTNESS 255
#define NUM_LEDS 240
CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* leds( leds_plus_safety_pixel + 1);

void setup() {
  // put your setup code here, to run once:	
	Serial.begin(9600);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness( BRIGHTNESS );
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 3)
  {
  	int red = Serial.parseInt();
  	int green = Serial.parseInt();
  	int blue = Serial.parseInt();
  	int brightness = Serial.parseInt();

  	Serial.print(red);
  	Serial.print(',');
  	Serial.print(green);
  	Serial.print(',');
  	Serial.print(blue);
  	Serial.print(',');
  	Serial.println(brightness);

  	for(int i = 0; i<NUM_LEDS; i++)
  		leds[ i ]  = CRGB( red, green, blue);

  	FastLED.setBrightness(brightness);
  FastLED.show();
  }
  
}