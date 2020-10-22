#include <Adafruit_NeoPixel.h>
#include <DFRobotDFPlayerMini.h>

/* Using Polulu 32U4 A-Star Micro
 * Pins:
 * GND - Power GND
 * 5V - POWER 5V
 * 0 = Serial1 Rx to DFPlayer Tx
 * 1 = Serial1 Tx to DFPlayer Rx
 * 2 = Light Trigger
 * 3 = Driver for light string 1 
 * 4 = Driver for light string 2
 * 
 * 6 = DFPlayer BUSY
 */

#define NSONGS 9

const uint16_t msFrameDuration = 0;
uint32_t frameNum = 0;

// LED strip vals/vars
const int addressesPerStrip = 100; // 3 LEDs per address
const int numStrips = 2;
const int pixelBufSize = numStrips * addressesPerStrip;
const uint8_t rainbowWidth = 8;
uint32_t pixels[pixelBufSize];
int16_t rainbowPos;
uint8_t randWheelStart;
uint8_t bkgdStart;
uint8_t bkgd_ctr = 0;
bool candyLightsInProgress = false;
// https://github.com/adafruit/Adafruit_NeoPixel
//   Parameter 1 = number of pixels in strip
//   Parameter 2 = pin number (most are valid)
//   Parameter 3 = pixel type flags, add together as needed:
//     NEO_RGB     Pixels are wired for RGB bitstream
//     NEO_GRB     Pixels are wired for GRB bitstream
//     NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//     NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(addressesPerStrip, 3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(addressesPerStrip, 4, NEO_GRB + NEO_KHZ800);

// sound vars
int input_led = 2;
int input_busy = 6;
int iSong = 1;
unsigned long ms_start;
volatile bool candySoundInProgress = false;  // TODO: Does this need to be volatile? // Covers sound-about-to-start, and sound-started states
volatile bool playing = false;  // TODO: Does this need to be volatile?
DFRobotDFPlayerMini mp3;


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(input_led, INPUT_PULLUP);
  pinMode(input_busy, INPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  //while (!Serial) {}
  //while (!Serial1) {}
  
  Serial.println("Initializing...");

  // init sound
  mp3.begin(Serial1);
  mp3.volume(24);

  pinMode(LED_BUILTIN, OUTPUT);

  // init LED strips
  randomSeed(analogRead(0));  
  strip0.begin();
  strip1.begin();
}

// infinite loop
void loop() {
  frameNum++;
  delay(msFrameDuration);

  updateBackgroundLayer();
  
  updateSparkles();  

  // will set candySoundInProgress and candyLightInProgress if neither already set and sensor tripped
  processCandySensor();

  if (candySoundInProgress) {
    updateCandySound();  
  }

  if (candyLightsInProgress) {
    updateCandyLights();  
  }

  updateLEDStrips();
}


void processCandySensor(void) {
  bool state = digitalRead(input_led); // false if beam broken
  digitalWrite(LED_BUILTIN, state);
  //Serial.print(state);
  if (!candySoundInProgress && !candyLightsInProgress && !state) {
    candySoundInProgress = true;
    candyLightsInProgress = true;
    rainbowPos = -rainbowWidth+1;
    randWheelStart = random(256);
  }
}

void updateCandySound(void) {
  bool not_busy = digitalRead(input_busy); // false if sound plaing

  if (!playing) {
    playing = true;
    mp3.playFolder(1, iSong+1);
    ms_start = millis();
    Serial.print("Started song ");
    Serial.println(iSong+1);
  }
  else {
    if ((millis() - ms_start > 1000) && not_busy) { // if sound finished
      playing = false;
      candySoundInProgress = false;
      iSong = (iSong + 1) % NSONGS;
      Serial.println(iSong);
    }
  }
}

void updateCandyLights(void) {
  updateCandyWavefront(rainbowPos, randWheelStart);

  if (frameNum % 1 == 0) {
    rainbowPos++;
    if (rainbowPos == pixelBufSize) {
      candyLightsInProgress = false;
    }
  }  
}

static uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
  // for some reason, green and blue are exchanged:
  // return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;  
  return ((uint32_t)r << 16) | ((uint32_t)b <<  8) | g;
}

void updateBackgroundLayer() {
  uint32_t c;
  for (uint16_t i=0; i<pixelBufSize/2; i++) {
    c = color(0, 0, 8 + i>>2);
    pixels[(i + bkgdStart) % pixelBufSize] = c;
    pixels[(pixelBufSize - i + bkgdStart) % pixelBufSize] = c;
  }

  bkgd_ctr++;
  if (bkgd_ctr >= 4) {
    bkgdStart = (bkgdStart + 1) % pixelBufSize;
    bkgd_ctr = 0;
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

void updateLEDStrips(void) {
  // obviously, this could be generalized for an array of strips:
  int i=0, j;
  for (j=0; j < addressesPerStrip; i++, j++) {
    strip0.setPixelColor(j, pixels[i]);
  }
  for (j=0; j < addressesPerStrip; i++, j++) {
    strip1.setPixelColor(j, pixels[i]);
  }

  strip0.show();
  strip1.show();
}

/*
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
*/
