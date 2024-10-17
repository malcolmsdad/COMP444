/*
  SparkFun Inventor’s Kit
  Circuit 4B - Temperature Sensor

  The LCD will display readings from a temperature sensor in degrees Celsius and Fahrenheit.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

#include <LiquidCrystal.h>                //the liquid crystal library contains commands for printing to the display
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  // tell the RedBoard what pins are connected to the display

float voltage = 0;   //the voltage measured from the TMP36
float degreesC = 0;  //the temperature in Celsius, calculated from the voltage
float degreesF = 0;  //the temperature in Fahrenheit, calculated from the voltage

int redPin = 4;
int greenPin = 5;
int bluePin = 6;

int lowTempC = 15;
int highTempC = 40;

int lowPot = 4;
int highPot = 290;

double showPotBarMode = 0;
double nextDrawTime = 0;

void setup() {

  lcd.begin(16, 2);  //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();       //clear the display

  //set the LED pins to output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // set up the photoresistor
  Serial.begin(9600);
}

// easy little helper to write to the led
void analogWriteRGB(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

//
void calcRgb(int percent) {
  switch (percent / 5) {
    case 0: analogWriteRGB(255, 0, 0); break;  // Red
    case 1: analogWriteRGB(255, 25, 0); break;
    case 2: analogWriteRGB(255, 51, 0); break;
    case 3: analogWriteRGB(255, 102, 0); break;
    case 4: analogWriteRGB(255, 153, 0); break;
    case 5: analogWriteRGB(255, 204, 0); break;  // Yellow
    case 6: analogWriteRGB(204, 255, 0); break;
    case 7: analogWriteRGB(153, 255, 0); break;
    case 8: analogWriteRGB(102, 255, 0); break;
    case 9: analogWriteRGB(51, 255, 0); break;
    case 10: analogWriteRGB(0, 255, 0); break;
    case 11: analogWriteRGB(51, 255, 0); break;
    case 12: analogWriteRGB(102, 255, 0); break;
    case 13: analogWriteRGB(153, 255, 0); break;
    case 14: analogWriteRGB(204, 255, 0); break;
    case 15: analogWriteRGB(255, 204, 0); break;  // Yellow
    case 16: analogWriteRGB(255, 153, 0); break;
    case 17: analogWriteRGB(255, 102, 0); break;
    case 18: analogWriteRGB(255, 51, 0); break;
    case 19: analogWriteRGB(255, 25, 0); break;
    case 20: analogWriteRGB(255, 0, 0); break;
    default: analogWriteRGB(0, 255, 0); break;  // Red  // Green
  }
}

void drawTempOnRgb() {
  double percent = double(degreesC - lowTempC) / double(highTempC - lowTempC);
  //lcd.print(percent);
  calcRgb(percent * 100);
}

int lastPot = 0;
void readPot() {
  int potRead = analogRead(A2);

  // if the last pot is more than 2 degrees of, set PotBar mode
  if (abs(potRead - lastPot) > 3) {
    showPotBarMode = millis() + 3000;  // show pot bar for 3 seconds
  }

  // record last reading
  lastPot = potRead;
}


int lastLight = 0;
void readLight() {
  int lightRead = analogRead(A1);

  // if the last pot is more than 2 degrees of, set PotBar mode
  if (abs(lightRead - lastLight) > 3) {
    showPotBarMode = millis() + 3000;  // show pot bar for 3 seconds
  }

  // record last reading
  lastLight = lightRead;
}

void drawTempOnLCD() {
  lcd.clear();  //clear the LCD

  lcd.setCursor(0, 0);       //set the cursor to the top left position
  lcd.print("Degrees C: ");  //print a label for the data
  lcd.print(degreesC);       //print the degrees Celsius

  lcd.setCursor(0, 1);       //set the cursor to the lower left position
  lcd.print("Degrees F: ");  //Print a label for the data
  lcd.print(degreesF);       //print the degrees Fahrenheit
}

void drawPotBars() {
  lcd.clear();  //clear the LCD

  //calc how many asterix to draw
  double percent = double(degreesC - lowTempC) / double(highTempC - lowTempC);
  int numberOf = percent * 14;  // there are 14 positions to draw

  lcd.setCursor(0, 0);  //set the cursor to the top left position
  lcd.print(lowTempC);
  for (int i = 0; i < 12; i++)
    lcd.print("-");
  lcd.print(highTempC);

  lcd.setCursor(0, 1);  //set the cursor to the lower left position
  lcd.print("[");
  // draw 14 stars or blanks
  for (int i = 0; i < numberOf; i++)
    lcd.print("*");

  // wow, why didn't I think of this earlier..
  lcd.setCursor(15, 1);
  lcd.print("]");
}

void loop() {
  // read temp
  voltage = analogRead(A0) * 0.004882813;    //convert the analog reading, which varies from 0 to 1023, back to a voltage value from 0-5 volts
  degreesC = (voltage - 0.5) * 100.0;        //convert the voltage to a temperature in degrees Celsius
  degreesF = degreesC * (9.0 / 5.0) + 32.0;  //convert the voltage to a temperature in degrees Fahrenheit

  // always draw rgb
  drawTempOnRgb();

  if (showPotBarMode > millis())
    drawPotBars();
  else
    drawTempOnLCD();


  // read the pot
  readPot();

  // read light
  readLight();

  delay(1000);  //delay for 1 second between each reading (this makes the display less noisy)
}
