/*
  SparkFun Inventor’s Kit
  Circuit 5A - Motor Basics

  Learn how to control one motor with the motor driver.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

//PIN VARIABLES
//the motor will be controlled by the motor A pins on the motor driver
const int AIN1 = 13;  //control pin 1 on the motor driver for the right motor
const int AIN2 = 12;  //control pin 2 on the motor driver for the right motor
const int PWMA = 11;  //speed control pin on the motor driver for the right motor

int switchPin = 7;  //switch to turn the robot on and off
int speedPin = 6;

//VARIABLES
int motorSpeed = 0;  //starting speed for the motor

void setup() {
  pinMode(switchPin, INPUT_PULLUP);  //set this as a pullup to sense whether the switch is flipped

  //set the motor control pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(speedPin, INPUT);

  Serial.begin(9600);  //begin serial communication with the computer

  Serial.println("Enter motor speed (0-255)... ");  //Prompt to get input in the serial monitor.
}

bool speedDown = false;
void checkSpeedButton() {
  if (digitalRead(speedPin) == LOW) {
    // button is down
    speedDown = true;
  } else {
    // Button released!  (last state was down, so trigger button release)
    if (speedDown) {
      motorSpeed += 50;

      // check for over speed, roll to zero
      if (motorSpeed > 254) motorSpeed = 0;

      Serial.println("Accelerator button pressed!");
      Serial.print("Motor Speed: ");  //print the speed that the motor is set to run at
      Serial.println(motorSpeed);
    }
    // button is up
    speedDown = false;
  }
}

void loop() {

  if (Serial.available() > 0) {      //if the user has entered something in the serial monitor
    motorSpeed = Serial.parseInt();  //set the motor speed equal to the number in the serial message

    Serial.print("Motor Speed: ");  //print the speed that the motor is set to run at
    Serial.println(motorSpeed);
  }

  // check speed button
  checkSpeedButton();

  // check for over speed, roll to zero
  if (motorSpeed > 254) motorSpeed = 0;

  // read photoresistor, if too low, set speed to zero
  int motorSwitch = (analogRead(A0) > 750) ? 0 : 1;

  // should we go in reverse?
  int reverseGear = (digitalRead(7) == LOW) ? 1 : -1;

  // set motor speed and direction
  spinMotor(motorSpeed * reverseGear * motorSwitch);
}

/********************************************************************************/
void spinMotor(int motorSpeed)  //function for driving the right motor
{

  if (motorSpeed > 0)  //if the motor should drive forward (positive speed)
  {
    digitalWrite(AIN1, HIGH);  //set pin 1 to high
    digitalWrite(AIN2, LOW);   //set pin 2 to low
  } else if (motorSpeed < 0)   //if the motor should drive backward (negative speed)
  {
    digitalWrite(AIN1, LOW);   //set pin 1 to low
    digitalWrite(AIN2, HIGH);  //set pin 2 to high
  } else                       //if the motor should stop
  {
    digitalWrite(AIN1, LOW);  //set pin 1 to low
    digitalWrite(AIN2, LOW);  //set pin 2 to low
  }
  analogWrite(PWMA, abs(motorSpeed));  //now that the motor direction is set, drive it at the entered speed
}
