#include <VirtualWire.h>
//#include <VirtualWire_Config.h>

#include <Adafruit_NeoPixel.h>


#include <avr/power.h>

// the value of the 'other' resistor 
#define SERIESRESISTOR 10000
// What pin to connect the sensor to
#define THERMISTORPIN A0
#define PIN 6

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
int numPixels = 240;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, PIN, NEO_GRB + NEO_KHZ800);
float  max = -100; //sensor initialization value
float  min = 500; //sensor initialization value


float N = 20;       //Input number of steps for transition

float color1_R = 0;
float color1_G = 10; //102
float color1_B = 20; //204

float color2_R = 10; //100
float color2_G = 0; //20
float color2_B = 0; //20

float a_R = (color2_R - color1_R) / N;
float a_G = (color2_G - color1_G) / N;
float a_B = (color2_B - color1_B) / N;

float driftFactor = 250;
const int numReadings = 5;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
bool full = false;
int ShrinkFactor = 0;

void setup() {
  // put your setup code here, to run once:
  // Start up the LED strip
  strip.begin();
 // analogReference(INTERNAL);
  // Update the strip, to start they are all 'off'
 // strip.show();
  Serial.begin(9600);
// Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec
  vw_rx_start();       // Start the receiver PLL running  
}

void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  unsigned short reading = 0;

  // subtract the last reading:
  delay (40);
  total = total - readings[readIndex];
  //reading = analogRead(THERMISTORPIN);
  boolean got_message = vw_get_message(buf, &buflen);
  while (!got_message) // Non-blocking
  {
      got_message = vw_get_message(buf, &buflen);
  }
  
  reading = 0;
  reading = buf[0];
  reading = reading | ((buf[1] & 0x7F) << 8);
  //uint8_t belt_id = buf[2];
  uint8_t belt_id = buf[1]>>7;
 // Serial.println(reading);
 // Serial.println(belt_id);

  readings[readIndex] = reading;
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    readIndex = 0;
    full = true;
  }
  average = total / numReadings;
  
  if (reading > max) {
    max = reading;
  }
  max = (max * driftFactor + average) / (driftFactor + 1);
  if (reading < min) {
    min = reading;
  }
  min = (min * driftFactor + average) / (driftFactor + 1);
  

  float breath = map(average, min, max, 0-ShrinkFactor, numPixels+ShrinkFactor);
  /*
  Serial.print("reading ");
  Serial.println(reading);
  Serial.print("max ");
  Serial.println(max);
  Serial.print("min ");
  Serial.println(min);
  Serial.print("average ");
  Serial.println(average);
  */
  Serial.print("breath ");
  Serial.println(breath);
 
  
 if (max>1000){
    uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    
  }
 }
else{ 
  for (int i = 0; i < numPixels; ++i) {
    if (i < breath) {
      strip.setPixelColor(numPixels-i, color1_R, color1_G, color1_B);
    } else if (i < (breath + N)) {
      float m = i - breath;
      strip.setPixelColor(numPixels - i, color1_R + m * a_R, color1_G + m * a_G, color1_B + m * a_B); // Set gradient pixel 'on'
    } else {
      strip.setPixelColor(numPixels - i, color2_R, color2_G, color2_B);      
    }    
  }
    strip.show();
    delay (20);
}

}


  // Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

