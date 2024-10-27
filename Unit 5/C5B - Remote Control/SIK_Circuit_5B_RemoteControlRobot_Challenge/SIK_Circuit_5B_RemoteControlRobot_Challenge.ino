/*
  SparkFun Inventor’s Kit
  Circuit 5B - Remote Control Robot

  Control a two wheeled robot by sending direction commands through the serial monitor.
  This sketch was adapted from one of the activities in the SparkFun Guide to Arduino.
  Check out the rest of the book at
  https://www.sparkfun.com/products/14326

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/


//the right motor will be controlled by the motor A pins on the motor driver
const int AIN1 = 13;  //control pin 1 on the motor driver for the right motor
const int AIN2 = 12;  //control pin 2 on the motor driver for the right motor
const int PWMA = 11;  //speed control pin on the motor driver for the right motor

//the left motor will be controlled by the motor B pins on the motor driver
const int PWMB = 10;  //speed control pin on the motor driver for the left motor
const int BIN2 = 9;   //control pin 2 on the motor driver for the left motor
const int BIN1 = 8;   //control pin 1 on the motor driver for the left motor

const double gainRightWheel = 0.95;  // reduce or gain by a factor of ...
const double gainLeftWheel = 1;      // reduce or gain by a factor of...

int switchPin = 7;  //switch to turn the robot on and off

const int driveTime = 20;  //this is the number of milliseconds that it takes the robot to drive 1 inch
                           //it is set so that if you tell the robot to drive forward 25 units, the robot drives about 25 inches

const int turnTime = 8;  //this is the number of milliseconds that it takes to turn the robot 1 degree
                         //it is set so that if you tell the robot to turn right 90 units, the robot turns about 90 degrees

//Note: these numbers will vary a little bit based on how you mount your motors, the friction of the
//surface that your driving on, and fluctuations in the power to the motors.
//You can change the driveTime and turnTime to make them more accurate

String botDirection;  //the direction that the robot will drive in (this change which direction the two motors spin in)
String distance;      //the distance to travel in each direction

/********************************************************************************/
void setup() {
  pinMode(switchPin, INPUT_PULLUP);  //set this as a pullup to sense whether the switch is flipped

  //set the motor control pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  Serial.begin(9600);  //begin serial communication with the computer

  //prompt the user to enter a command
  Serial.println("Enter a direction followed by a distance.");
  Serial.println("f = forward, b = backward, r = turn right, l = turn left");
  Serial.println("Example command: f 50");
}

/********************************************************************************/
void loop() {
  if (digitalRead(7) == LOW) {   //if the switch is in the ON position
    if (Serial.available() > 0)  //if the user has sent a command to the RedBoard
    {
      botDirection = Serial.readStringUntil(' ');  //read the characters in the command until you reach the first space
      distance = Serial.readStringUntil(' ');      //read the characters in the command until you reach the second space

      //print the command that was just received in the serial monitor
      Serial.print(botDirection);
      Serial.print(" ");
      Serial.println(distance.toInt());

      if (botDirection == "f") driveForward(driveTime * distance.toInt());
      else if (botDirection == "b") driveBackward(driveTime * distance.toInt());
      else if (botDirection == "r") driveRight(driveTime * distance.toInt());
      else if (botDirection == "l") driveLeft(driveTime * distance.toInt());
      else if (botDirection == "z") driveZigZag(driveTime * distance.toInt());
    } else {
      rightMotor(0);  //turn the right motor off
      leftMotor(0);   //turn the left motor off
    }
  }
}
void stop(int pause = 0) {
  rightMotor(0);
  leftMotor(0);
  delay(pause);
}


void driveLeft(int distance) {
  rightMotor(255);  //drive the right wheel forward
  leftMotor(-200);  //drive the left wheel forward
  delay(distance);  //drive the motors long enough turn the entered distance
  rightMotor(0);    //turn the right motor off
  leftMotor(0);     //turn the left motor off
}

void driveRight(int distance) {
  rightMotor(-200);  //drive the right wheel forward
  leftMotor(255);    //drive the left wheel forward
  delay(distance);   //drive the motors long enough turn the entered distance
  rightMotor(0);     //turn the right motor off
  leftMotor(0);      //turn the left motor off
}

void driveBackward(int distance) {
  rightMotor(-200);  //drive the right wheel forward
  leftMotor(-200);   //drive the left wheel forward
  delay(distance);   //drive the motors long enough travel the entered distance
  rightMotor(0);     //turn the right motor off
  leftMotor(0);      //turn the left motor off
}

void driveForward(int distance) {
  rightMotor(200);  //drive the right wheel forward
  leftMotor(200);   //drive the left wheel forward
  delay(distance);          //drive the motors long enough travel the entered distance
  rightMotor(0);    //turn the right motor off
  leftMotor(0);     //turn the left motor off
}

void wiggleWheel()
{
  driveLeft(10);
  driveRight(10);
  driveBackward(10);
  driveForward(20);
}

void driveZigZag(int distance) {

  Serial.write("Zig zag avoidance maneouver..");
  driveForward(driveTime * 10);
  driveLeft(driveTime * 20);
  driveForward(driveTime * 50);
  driveRight(driveTime * 10);
  driveForward(driveTime * 50);
  driveRight(driveTime * 20);
  driveForward(driveTime * 50);
  driveLeft(driveTime * 20);
  driveForward(driveTime * 25);
}


/********************************************************************************/
void rightMotor(int motorSpeed)  //function for driving the right motor
{
  int gainSpeed = motorSpeed * gainRightWheel;
  if (gainSpeed > 0)  //if the motor should drive forward (positive speed)
  {
    digitalWrite(AIN1, HIGH);  //set pin 1 to high
    digitalWrite(AIN2, LOW);   //set pin 2 to low
  } else if (gainSpeed < 0)    //if the motor should drive backward (negative speed)
  {
    digitalWrite(AIN1, LOW);   //set pin 1 to low
    digitalWrite(AIN2, HIGH);  //set pin 2 to high
  } else                       //if the motor should stop
  {
    digitalWrite(AIN1, LOW);  //set pin 1 to low
    digitalWrite(AIN2, LOW);  //set pin 2 to low
  }
  analogWrite(PWMA, abs(gainSpeed));  //now that the motor direction is set, drive it at the entered speed
}

/********************************************************************************/
void leftMotor(int motorSpeed)  //function for driving the left motor
{
  int gainSpeed = motorSpeed * gainLeftWheel;

  if (gainSpeed > 0)  //if the motor should drive forward (positive speed)
  {
    digitalWrite(BIN1, HIGH);  //set pin 1 to high
    digitalWrite(BIN2, LOW);   //set pin 2 to low
  } else if (gainSpeed < 0)    //if the motor should drive backward (negative speed)
  {
    digitalWrite(BIN1, LOW);   //set pin 1 to low
    digitalWrite(BIN2, HIGH);  //set pin 2 to high
  } else                       //if the motor should stop
  {
    digitalWrite(BIN1, LOW);  //set pin 1 to low
    digitalWrite(BIN2, LOW);  //set pin 2 to low
  }
  analogWrite(PWMB, abs(gainSpeed));  //now that the motor direction is set, drive it at the entered speed
}
