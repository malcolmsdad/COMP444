/*
  Sp4rkFUn Inv3nt0r’s K1t  ---- HACKED BY TIM BLAKE TO Samuel Morse! 

  Turns your SIK into a life saving device, for anyone doing CMP 444 on the open ocean.

*/

void setup() {

  pinMode(13, OUTPUT);      // Set pin 13 to output

}

int unitOfTime = 150; // Units of time, in milliseconds
int dotLength = 1;    // How long to show a dot for? 1 unit of time
int dashLength = 3;   // How long to show a dash for? 3 units...
int pauseLength = 1;  // How long do we wait better letters?
int wordLength = 3;   // How long do we wait between words?

void dash (){
  digitalWrite(13, HIGH);            // Turn on the LED
  delay(dashLength*unitOfTime);      // Wait for DASH units of time X UNITS of time (ms)
  digitalWrite(13, LOW);             // Turn off the LED
  delay(pauseLength*unitOfTime);     // Wait for pause units of time X UNITS of time (ms)
}

void dot (){
  digitalWrite(13, HIGH);             // Turn on the LED
  delay(dotLength*unitOfTime);        // Wait for two seconds
  digitalWrite(13, LOW);              // Turn on the LED
  delay(pauseLength*unitOfTime);      // Wait for two seconds
}

void nextLetter()
{
  digitalWrite(13, LOW);   // Turn on the LED
  delay(wordLength*unitOfTime);
}

void loop() {

// S is 3 dashes
  dash();       
  dash();
  dash();
  nextLetter();

// O is 3 dots
  dot();
  dot();
  dot();
  nextLetter();

// S is 3 dashes again
  dash();
  dash();
  dash();

// take a quick nap
  nextLetter();
  nextLetter();
}

