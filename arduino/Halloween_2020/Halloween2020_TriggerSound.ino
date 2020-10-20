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

#define NSONGS  9
DFRobotDFPlayerMini mp3;

int input_led = 2;
int input_busy = 6;
int iSong = 1;
unsigned long ms_start;
volatile bool triggered = false;
volatile bool playing = false;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(input_led, INPUT_PULLUP);
  pinMode(input_busy, INPUT);
  Serial.begin(9600);
  Serial1.begin(9600);
  mp3.begin(Serial1);
  mp3.volume(24);
}

// the loop function runs over and over again forever
void loop() {
  update_trigger();
  delay(10);
}

void update_trigger(void) {
  bool state = digitalRead(input_led);
  bool not_busy = digitalRead(input_busy);
  digitalWrite(LED_BUILTIN, state);
  if (!triggered && !state) {
    triggered = true;  
  }
  else if (triggered && !playing) {
    Serial.print("Started song ");
    Serial.println(iSong+1);
    mp3.playFolder(1, iSong+1);
    playing = true;
    ms_start = millis();
  }
  else if (triggered && playing) {
   if ((millis() - ms_start > 1000) && (not_busy)) {
    playing = false;
    triggered = false;
    iSong = (iSong + 1) % NSONGS;
    Serial.println(iSong);
   }
  }
}
