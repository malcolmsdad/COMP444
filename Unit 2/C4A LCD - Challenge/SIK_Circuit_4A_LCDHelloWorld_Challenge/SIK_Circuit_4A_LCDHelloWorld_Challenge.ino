/*
  SparkFun Inventor’s Kit
  Circuit 4A-HelloWorld

  The LCD will display the words "Hello World" and show how many seconds have passed since
  the RedBoard was last reset.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

#include <LiquidCrystal.h>  //the liquid crystal library contains commands for printing to the display
#include <time.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  // tell the RedBoard what pins are connected to the display

String line1Eng = "Time:    ";  // english first line message
String line1Fre = "Temps:   ";  // french
String line2Eng = "Thanks";  // english second line message
String line2Fre = "Merci ";  // french

bool isEnglish = true;  // default english language

int btnCount = 0;             // count the number of times the button was pressed
long thanksUntil = millis();  // Until what millisecond do we hide exclamation marks for?
int lastM = 0;
int btnPin = 2;

void setup() {

  lcd.begin(16, 2);  //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();       //clear the display
  pinMode(btnPin, INPUT_PULLUP);
}

// button press event
void btnPress() {
  // increase button count
  btnCount++;
  // set thanks exclamation mark count to 3,000 milliseconds or 3 seconds (but not higher!)
  thanksUntil = millis() + 3000;
  // just for fun, switch language
  isEnglish = !isEnglish;
}

// Helper function for generating first line, I don't like cluttering up the main loop
String firstLineMessage() {
  char buff[80];
  time_t rawtime;
  strftime(buff, 80, "%R", localtime(&rawtime));
  return (((isEnglish) ? line1Eng : line1Fre) + buff);
}

void left_fill_zeros(char* dest, const char* str, int length) {
  sprintf(dest, "%.*d%s", (int)(length - strlen(str)), 0, str);
}

String secondLineMessage() {
  // init string
  String returnString = "";

  // do we say thanks?
  if (thanksUntil > millis())
    returnString = (isEnglish) ? line2Eng : line2Fre;
  else
    returnString = "      ";  // we need to pad with white space!

  // do we show any exclamation marks?
  int excCount = (thanksUntil - millis()) / 1000;
  for (int i = 0; i < excCount; i++)
    returnString += "!";

  // now everything must line up
  for (int i = min(3, max(0, excCount)); i <= 3; i++)
    returnString += " ";

  // so far, we are at 9 characters. we want three at the end for the button count, so we need a gap of 4 spaces:
  returnString += "   ";

  // now display the number of button presses
  if (btnCount < 100) returnString += " ";  // add padding
  if (btnCount < 10) returnString += " ";   // add padding
  return returnString + btnCount;

  //return returnString;
}

bool btnDown = false;
void loop() {

  // is the button up?
  if (digitalRead(btnPin) == HIGH) {
    btnDown = false;
  }

  // check for button press
  if (digitalRead(btnPin) == LOW && !btnDown) {
    btnPress();
    btnDown = true;
  }

  // first line
  lcd.setCursor(0, 0);            //set the cursor to the 0,0 position (top left corner)
  lcd.print(firstLineMessage());  //print hello, world! starting at that position

  // second line
  lcd.setCursor(0, 1);             //move the cursor to the first space of the bottom row
  lcd.print(secondLineMessage());  //print the number of seconds that have passed since the last reset
}
