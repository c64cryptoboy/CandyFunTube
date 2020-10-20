#include <Adafruit_NeoPixel.h>

const uint16_t msFrameDuration = 10;
const int addressesPerStrip = 100; // 3 LEDs per address
const int numStrips = 2;
const int pixelBufSize = numStrips * addressesPerStrip;
const uint8_t rainbowWidth = 8;

// https://github.com/adafruit/Adafruit_NeoPixel

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(addressesPerStrip, 3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(addressesPerStrip, 4, NEO_GRB + NEO_KHZ800);

uint32_t pixels[pixelBufSize];
uint32_t frameNum;
int16_t rainbowPos;
uint8_t randWheelStart;
bool candyMode = false;

// setup runs after each powerup or reset of the Arduino board
void setup() {
  frameNum = 0;
  rainbowPos = -rainbowWidth+1;
  candyMode = false;
  randomSeed(analogRead(0));
  Serial.begin(9600);
  Serial.println("Initializing...");
  
  strip0.begin();
  strip1.begin();
}

// infinite loop
void loop() {
  frameNum++;
  delay(msFrameDuration);

  updateBackgroundLayer();
  updateSparkles();  

  // if (!candyMode && !digitalRead(2)) {
  if (!candyMode) {  // TODO: sensor not present yet
     rainbowPos = -rainbowWidth+1;
     candyMode = true;
     randWheelStart = random(256);  
  }
  if (candyMode) {
    updateCandyWavefront(rainbowPos, randWheelStart);
    //debugPixels(); delay(1000);

    if (frameNum % 1 == 0) {
      rainbowPos++;
      if (rainbowPos == pixelBufSize) {
        rainbowPos = -rainbowWidth+1;
        candyMode = false;
      }
    }
  }

  // obviously, this could be generalized for an array of strips:
  for (int i=0*addressesPerStrip; i<1*addressesPerStrip; i++) {
    strip0.setPixelColor(i, pixels[i]);
  }
  for (int i=1*addressesPerStrip; i<2*addressesPerStrip; i++) {
    strip1.setPixelColor(i, pixels[i]);
  }

  strip0.show();
  strip1.show();  
}

static uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
  // for some reason, green and blue are exchanged:
  // return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;  
  return ((uint32_t)r << 16) | ((uint32_t)b <<  8) | g;
}

void debugPixels() {
  for (uint16_t i=0; i<numStrips; i++) {
    Serial.println();
    for (uint16_t j=0; j<addressesPerStrip; j++) {
      Serial.print(pixels[i * addressesPerStrip + j]);
      Serial.print(" ");
    }
  }
  Serial.println();
}

void updateBackgroundLayer() {
  uint8_t r, g, b;
  uint32_t c;

  // for now, this is a fixed blue background
  r = g = 0;
  b = 16;
  c = color(r, g, b);

  for (uint16_t i=0; i<pixelBufSize; i++) {
    pixels[i] = c;
  }
}

void updateSparkles() {
  uint16_t odds;

  odds = random(100);
  if (odds < 10) {
     pixels[random(pixelBufSize)] = color(255, 255, 255);
  }
}

void updateCandyWavefront(int16_t rainbowPos, uint8_t randWheelStart) {
  for (int i = max(0, rainbowPos); i <= min(pixelBufSize, rainbowPos+rainbowWidth); i++) {
    pixels[i] = wheel((2 * i + randWheelStart) % 256);
  }
}

uint32_t wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return color(255 - WheelPos * 3, WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return color(0, 255 - WheelPos * 3, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return color(WheelPos * 3, 0, 255 - WheelPos * 3);
  }
}

/*
void testStuff(Adafruit_NeoPixel *aStrip) {
  Serial.println("newtest");
  Serial.println((*aStrip).numPixels());
}
*/
