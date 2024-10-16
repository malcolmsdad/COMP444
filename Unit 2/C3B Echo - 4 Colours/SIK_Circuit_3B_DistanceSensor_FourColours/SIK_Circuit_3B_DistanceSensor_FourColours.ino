/*
  SparkFun Inventor’s Kit
  Circuit 3B-Distance Sensor

  Control the color of an RGB LED using an ultrasonic distance sensor.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

const int trigPin = 11;  //connects to the trigger pin on the distance sensor
const int echoPin = 12;  //connects to the echo pin on the distance sensor

const int redPin = 3;    //pin to control the red LED inside the RGB LED
const int greenPin = 5;  //pin to control the green LED inside the RGB LED
const int bluePin = 6;   //pin to control the blue LED inside the RGB LED

const String unit = "in";  // "cm" for centimeters, "in" for inches
                           //= "cm";


// create a distance range.. and assign colors based on that range.
float maxDistance = 12;  // show a green colour for this value
                         // show a yellow value for something in between
float minDistance = 1;   // show a red colour for this value

float distance = 0;  //stores the distance measured by the distance sensor

void setup() {
  Serial.begin(9600);  //set up a serial connection with the computer

  pinMode(trigPin, OUTPUT);  //the trigger pin will output pulses of electricity
  pinMode(echoPin, INPUT);   //the echo pin will measure the duration of pulses coming back from the distance sensor

  //set the RGB LED pins to output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

// easy little helper to write to the led
void analogWriteRGB(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void loop() {
  distance = getDistance();  //variable to store the distance measured by the sensor

  Serial.print(distance);      //print the distance that was measured
  Serial.println(" " + unit);  //print units after the distance

  // setup rgb vals
  int redValue = 0;
  int blueValue = 0;
  int greenValue = 0;

  // find the period of the range over 10 segments (that's how many rgb values I have)
  double period = (maxDistance - minDistance) / 10;

  // using an int here, because I want to drop the decimals! tricky eh
  int segment = distance / period;

  // depending on what segment of the gradient we are, we display different colour using our handy RGB write function.
  switch (segment) {
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
    default: analogWriteRGB(0, 255, 0); break;  // Green
  }

  //make the RGB LED red



  delay(50);  //delay 50ms between each reading
}

//------------------FUNCTIONS-------------------------------

//RETURNS THE DISTANCE MEASURED BY THE HC-SR04 DISTANCE SENSOR
float getDistance() {
  float echoTime;            //variable to store the time it takes for a ping to bounce off an object
  float calculatedDistance;  //variable to store the distance calculated from the echo time

  //send out an ultrasonic pulse that's 10ms long
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  echoTime = pulseIn(echoPin, HIGH);  //use the pulsein command to see how long it takes for the
                                      //pulse to bounce back to the sensor

  if (unit == "cm")
    calculatedDistance = echoTime / 58.0;  //Sourced from sensors datasheet hosted here: https://www.handsontec.com/dataspecs/HC-SR04-Ultrasonic.pdf
  else
    calculatedDistance = echoTime / 148.0;  //calculate the distance of the object that reflected the pulse (half the bounce time multiplied by the speed of sound)

  return calculatedDistance;  //send back the distance that was calculated
}
