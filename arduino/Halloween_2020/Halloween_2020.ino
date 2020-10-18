#include <Adafruit_NeoPixel.h>

// https://github.com/adafruit/Adafruit_NeoPixel

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(100, 3, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  randomSeed(analogRead(0));
}

void loop() {
  // rainbowCycle(1);
  sparkleWhite(10);
  //runner(40);
  clearAll();
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*10; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(0, 255 - WheelPos * 3, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 0, 255 - WheelPos * 3);
  }
}

void runner(uint8_t wait)
{
  uint16_t np = strip.numPixels();
  uint32_t color;
  for (uint16_t j=0; j<4; j++) {
    color = Wheel((j * 16) % 256);
    for (uint16_t i=0; i < np; i++) {
      clearAll();
      strip.setPixelColor(i, color);
      strip.show();
      delay(wait);
    }
  }
}
    
void fromCenter(uint8_t wait) 
{
  uint16_t np = strip.numPixels()/2;
  uint32_t color;
  for (uint16_t j=0; j<16; j++) {
    clearAll();
    color = Wheel((j * 16) % 256);
    for (uint16_t i=0; i < np; i++) {
      strip.setPixelColor(np + i, color);
      strip.setPixelColor(np - i, color);
      strip.show();
      delay(wait);
    }
  }
}

void sparkle(uint8_t wait) 
{
  uint32_t color;
  uint16_t pos;
  uint16_t nOn = strip.numPixels() / 3;
  for (uint16_t j=0; j<256; j++) {
    clearAll();
    for (uint16_t k=0; k<nOn; k++) {
      pos = random(strip.numPixels());
      color = Wheel(random(256));
      strip.setPixelColor(pos, color);
    }
    strip.show();
    delay(wait);
  }
}

void sparkleWhite(uint8_t wait) 
{
  uint32_t color;
  uint16_t pos, odds;
  uint16_t nOn = 1; //strip.numPixels() / 50;
  for (uint16_t j=0; j<256; j++) {
    odds = random(100);
    setAll(strip.Color(0, 16, 0));
    if (odds < 20) {
    for (uint16_t k=0; k<nOn; k++) {
      pos = random(strip.numPixels());
      color = strip.Color(255, 255, 255);
      strip.setPixelColor(pos, color);
    }
    }
    strip.show();
    delay(wait);
  }
}       

void setAll(uint32_t c)
{
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
}

void clearAll(void) 
{
  uint32_t black = strip.Color(0, 0, 0);
  for (int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, black);
  }
}
