/*
  SparkFun Inventor’s Kit
  Circuit 3A-Servo

  Move a servo attached to pin 9 so that it's angle matches a potentiometer attached to A0.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

#include <Servo.h>          //include the servo library

int potPosition;           //this variable will store the position of the potentiometer
int servoPosition;         //the servo will move to this position

int minServoPosition = 100; // the minimum degree position the servo can move to. Default was 20.  The higher the number, the smaller the range
int maxServoPosition = 110; // the maximuim degree position the servo can move to. Default was 160.  The lower the number, the smaller the range. 
                            // The program will santize these values and no permit under rotation, over rotation, over invalid input, ie: min > max.

Servo myservo;              //create a servo object

void setup() {

  myservo.attach(9);        //tell the servo object that its servo is plugged into pin 9

}

void loop() {



  potPosition = analogRead(A0);                     //use analog read to measure the position of the potentiometer (0-1023)

  // clean inputs variables
  int minP = min(minServoPosition, maxServoPosition); // ensure we get the lower value of the two.. dont want no tricksters here
  int maxP = max(minServoPosition, maxServoPosition); // ensure we get the higher value...

  // protect the device
  minP = max(20, minP);  // do not let values below 20 
  maxP = min(160, maxP);  // do not let values above 160

  servoPosition = map(potPosition, 0, 1023, minP, maxP); //convert the potentiometer number to a servo position from 20-160
                                                      //Note: its best to avoid driving the little SIK servos all the
                                                      //way to 0 or 180 degrees it can cause the motor to jitter, which is bad for the servo.

  myservo.write(servoPosition);                      //move the servo to the 10 degree position
}
