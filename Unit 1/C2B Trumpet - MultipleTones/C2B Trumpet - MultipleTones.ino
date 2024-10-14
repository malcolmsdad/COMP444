/*
  SparkFun Inventor’s Kit
  Circuit 2B-ButtonTrumpet

  Use 3 buttons plugged to play musical notes on a buzzer.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/
#include <string.h>

//set the pins for the button and buzzer
int firstKeyPin = 2;
int secondKeyPin = 3;
int thirdKeyPin = 4;

int buzzerPin = 10;

const int toneArray[8] = {262,294,330,349,392,440,494,523};


void setup() {
  //set the button pins as inputs
  pinMode(firstKeyPin, INPUT_PULLUP);
  pinMode(secondKeyPin, INPUT_PULLUP);
  pinMode(thirdKeyPin, INPUT_PULLUP);

  //set the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);
}

void loop() {

  // set up booleans to track button presses
  bool greenBtn = false;
  bool yellowBtn = false;
  bool redBtn = false;

  // get states of buttons
  greenBtn = (digitalRead(firstKeyPin) == LOW);
  yellowBtn = (digitalRead(secondKeyPin) == LOW);
  redBtn = (digitalRead(thirdKeyPin) == LOW);

  // calculate position of tone
  int tonePosition = 0;
  if     (redBtn   &  greenBtn   &  yellowBtn ) tonePosition = 1;
  else if (!redBtn  &  greenBtn   &  yellowBtn ) tonePosition = 2;
  else if (redBtn   &  !greenBtn  &  yellowBtn ) tonePosition = 3;
  else if (!redBtn  &  !greenBtn  &  yellowBtn ) tonePosition = 4;
  else if (redBtn   &  greenBtn   &  !yellowBtn) tonePosition = 5;
  else if (!redBtn  &  greenBtn   &  !yellowBtn) tonePosition = 6;
  else if (redBtn   &  !greenBtn  &  !yellowBtn) tonePosition = 7;
  else if (!redBtn  &  !greenBtn  &  !yellowBtn) tonePosition = 0;

  if (tonePosition > 0) {
    Serial.print(tonePosition);
    tone(buzzerPin, toneArray[tonePosition]);
  }
  else {
    noTone(buzzerPin);                        //if no key is pressed turn the buzzer off
  }
}

