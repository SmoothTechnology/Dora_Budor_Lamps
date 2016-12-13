#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_PIN  11

#define COLOR_ORDER GRB
#define CHIPSET     WS2811

#define NUM_LEDS 240

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_RGBW + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:	
	Serial.begin(9600);
    pixels.begin(); // This initializes the NeoPixel library.
  pixels.show();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 2)
  {
  	int red = Serial.parseInt();
  	int green = Serial.parseInt();
  	int blue = Serial.parseInt();

  	Serial.print(red);
  	Serial.print(',');
  	Serial.print(green);
  	Serial.print(',');
  	Serial.println(blue);

  	for(int i = 0; i<NUM_LEDS; i++)
  		pixels.setPixelColor(i, pixels.Color(green, red, blue));

  
  }

  pixels.show();
  
}