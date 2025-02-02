#include <Wire.h>
#include "Adafruit_MPU6050.h"
#include "Adafruit_Sensor.h"

Adafruit_MPU6050 mpu;

#define PWM_DRIVE_PIN 5
#define MTR_DRIVE_PIN1 6  //6
#define MTR_DRIVE_PIN2 7  //7

#define PWM_STEER_PIN 3
#define MTR_STEER_PIN1 2  //1
#define MTR_STEER_PIN2 1  //2

#define STDBY_PIN 4

#define TRIG_PIN_FRONT 9
#define ECHO_PIN_FRONT 8
#define TRIG_PIN_LEFT 13
#define ECHO_PIN_LEFT 12
#define TRIG_PIN_LEFT_45 11
#define ECHO_PIN_LEFT_45 10

// RANGE CONSTANTS
const int DIST_LEFT_FAR = 50;
const int DIST_LEFT_NEAR = 30;
const int DIST_LEFT_CLOSE = 10;

const int DIST_FRONT_FAR = 300;
const int DIST_FRONT_NEAR = 50;
const int DIST_FRONT_CLOSE = 5;

// SPEED CONSTANTS
const int MAX_SPEED = 255;             // PWM value for full speed
const int MIN_SPEED = 0;               // PWM value for stop
const int REV_SPEED = -255;            // REVERSE Speed
const int REDUCE_SPEED_PERCENT = 25;   // Speed reduction percentage
const int REVERSE_SPEED_PERCENT = 50;  // Reverse speed percentage
const int ACCELERATION_STEP = 5;       // Acceleration step for gradual increase

// DISTANCE VARIABLES
long prevDistanceFront = 0;
long distanceFront = 20;

long prevDistance45 = 0;
long distanceLeft45 = 20;

long prevDistanceLeft = 0;
long distanceLeft = 20;

long forwardSpeed = 0;
long steerDirection = 0;

// drive stats
long cmdDrive = 10;
long prevDrive = 10;
long cmdSteer = 0;

long prevDrive3 = 0;
long prevDrive2 = 0;
long prevDrive1 = 0;

String reverseChar = "";
String coastChar = "";

float velocityX = 0;
float velocityY = 0;

float gyroSpeed = 0;

unsigned long lastTime;

// Calibration variables
float accelXOffset = 0;
float accelYOffset = 0;
float accelZOffset = 0;

void calibrateMPU(int calibrationSamples = 500) {
    Serial.println("Calibrating MPU6050...");
    float totalAccelX = 0;
    float totalAccelY = 0;
    float totalAccelZ = 0;

    for (int i = 0; i < calibrationSamples; i++) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        totalAccelX += a.acceleration.x;
        totalAccelY += a.acceleration.y;
        totalAccelZ += a.acceleration.z;

        delay(10);
    }

    accelXOffset = totalAccelX / calibrationSamples;
    accelYOffset = totalAccelY / calibrationSamples;
    accelZOffset = (totalAccelZ / calibrationSamples) - 9.81; // Adjust for gravity

    Serial.println("Calibration complete.");
}


void setupMPU(){
    // Initialize MPU6050
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
            delay(10);
        }
    }

    // Set ranges and filter bandwidth
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ); 

  calibrateMPU(500);

    lastTime = millis(); 
}

void setup() {
  pinMode(PWM_DRIVE_PIN, OUTPUT);
  pinMode(PWM_STEER_PIN, OUTPUT);

  pinMode(MTR_DRIVE_PIN1, OUTPUT);
  pinMode(MTR_DRIVE_PIN2, OUTPUT);
  pinMode(MTR_STEER_PIN1, OUTPUT);
  pinMode(MTR_STEER_PIN2, OUTPUT);

  pinMode(STDBY_PIN, OUTPUT);

  pinMode(TRIG_PIN_FRONT, OUTPUT);
  pinMode(ECHO_PIN_FRONT, INPUT);
  pinMode(TRIG_PIN_LEFT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  pinMode(TRIG_PIN_LEFT_45, OUTPUT);
  pinMode(ECHO_PIN_LEFT_45, INPUT);

  setupMPU();

  digitalWrite(STDBY_PIN, HIGH);  // Enable motor driver standby mode
  Serial.begin(9600);             // Initialize serial communication
}


void calculateGyroSpeed(){
      sensors_event_t a, g, temp;

    // Get new sensor events
    mpu.getEvent(&a, &g, &temp);

    unsigned long currentTime = millis();
    float dt = (currentTime - lastTime) / 1000.0; // Time difference in seconds
    lastTime = currentTime;

    // Calculate acceleration
    float ax = a.acceleration.x - accelXOffset;
    float ay = a.acceleration.y - accelYOffset;

    // Calculate velocity by integrating acceleration
    if (ax > 0)
      velocityX = floor(ax * dt * 4) / 4.0;
    else
      velocityX = ceil(ax * dt * 2) / 2.0;

    // Calculate the resultant speed in the X and Y plane
    gyroSpeed += velocityX;

    Serial.print("~");
    Serial.print(ax);
    Serial.print("`");
    Serial.print(velocityX);
    Serial.print("`");
    Serial.print(gyroSpeed);
    Serial.print("~");
}

void calculateMotorSpeed() {

  // current condition, going forward or back?

  // in reverse..
  if (prevDrive < 0) {
    if (distanceFront < 25) cmdDrive = TranslateSpeedExp(-3);
    else if (distanceFront < 35) cmdDrive = TranslateSpeedExp(-1);
    else cmdDrive = TranslateSpeedExp(1);
    return;
  } else {
    // increase forward buffer
    long testDistance = distanceFront + max(30, forwardSpeed);

    // going forward
    if (testDistance < 40 || distanceFront < 20) cmdDrive = TranslateSpeedExp(-3);
    else if (testDistance < 50) cmdDrive = TranslateSpeedExp(-1);
    else if (testDistance < 60 || forwardSpeed < 5) cmdDrive = TranslateSpeedExp(3);
    else if (testDistance < 85) cmdDrive = TranslateSpeedExp(2);
    else if (testDistance < 85) cmdDrive = TranslateSpeedExp(1);
  }
}

long prevSteer = 0;
void calculateSteerDirection(long direction) {

  // current condition, going forward or back?
  if (prevDrive < 0) {  // going backward.. so keep that direction a bit longer
    cmdSteer = TranslateSpeedExp(-3);
  }

  // going forward
  if (distanceLeft < 30 || distanceLeft45 < 40)
    cmdSteer = TranslateSpeedExp(3);
  else if (distanceLeft > 45 || distanceLeft45 > 55)
    cmdSteer = TranslateSpeedExp(-3);
  else
    cmdSteer = TranslateSpeedExp(0);
}

String formatLong(long value, char pad = '0') {
  String formattedValue;

  // Add the sign
  if (value >= 0) {
    formattedValue += " ";
  } else
    formattedValue += "-";

  // Pad the value to 3 digits
  if (value >= -9 && value <= 9) {
    formattedValue += "  ";
  } else if (value >= -99 && value <= 99) {
    formattedValue += " ";
  }

  formattedValue += abs(value);

  return formattedValue;
}

void outputState() {
  Serial.print("[FR:");
  Serial.print(formatLong(distanceFront, ' '));

  Serial.print("] [45:");
  Serial.print(formatLong(distanceLeft45, ' '));

  Serial.print("] [LF:");
  Serial.print(formatLong(distanceLeft, ' '));

  Serial.print("] - [GAS:");
  Serial.print(formatLong(cmdDrive, ' '));

  Serial.print("] [STR:");
  Serial.print(formatLong(cmdSteer, ' '));

  Serial.print("] - [");
  Serial.print(reverseChar);

  Serial.print("] [");
  Serial.print(coastChar);

  Serial.print("] <");
  Serial.print(gyroSpeed);

  Serial.println(">");
}

void driveMotor() {

  // reset coast indicator
  coastChar = "  ";
  reverseChar = "  ";

  // check max speed!!  if prevSpeed[1,2,3] > 500 SLOW DOWN
  cmdDrive = min(cmdDrive, 255);
  if (cmdDrive > 0 && forwardSpeed > 2) {
    cmdDrive = 234;
    coastChar = "CST";
  }

  // if switch direction? pause for a second
  if (prevDrive > 0 && cmdDrive < 0) {
    // set reverse status
    reverseChar = "REV";

    // don't stress the motors, and nap
    digitalWrite(MTR_DRIVE_PIN1, LOW);
    digitalWrite(MTR_DRIVE_PIN2, LOW);
    analogWrite(PWM_DRIVE_PIN, 0);

    // recalibrate gyro
    //calibrateMPU(100);
  }

  // set motor speed
  analogWrite(PWM_DRIVE_PIN, abs(cmdDrive));

  // which direction are we going
  if (cmdDrive > 0) {
    digitalWrite(MTR_DRIVE_PIN1, HIGH);
    digitalWrite(MTR_DRIVE_PIN2, LOW);
  } else {
    digitalWrite(MTR_DRIVE_PIN1, LOW);
    digitalWrite(MTR_DRIVE_PIN2, HIGH);
  }

  // store previous speeds
  prevDrive3 = prevDrive2;
  prevDrive2 = prevDrive1;
  prevDrive1 = prevDrive;
  prevDrive = cmdDrive;
}

long prevDirection = 0;
void steerMotor() {

  long tmpSteer = abs(cmdSteer);
  if (tmpSteer > MAX_SPEED) tmpSteer = MAX_SPEED;

  // set steer speed
  if (prevDirection != cmdSteer) {
    digitalWrite(MTR_STEER_PIN1, LOW);
    digitalWrite(MTR_STEER_PIN2, LOW);
    analogWrite(PWM_STEER_PIN, tmpSteer);
  }

  // which direction do we steer?
  if (cmdSteer > 0) {
    digitalWrite(MTR_STEER_PIN2, LOW);
    digitalWrite(MTR_STEER_PIN1, HIGH);
  } else if (cmdSteer < 0) {
    digitalWrite(MTR_STEER_PIN1, LOW);
    digitalWrite(MTR_STEER_PIN2, HIGH);
  }

  //steering slows the motor down!
  if (tmpSteer >= 245)
    cmdDrive = 245;

  // record previous speed
  prevDirection = cmdSteer;
}

void resumeMotors() {
  driveMotor();
  steerMotor();
}


long readDistance(int trigPin, int echoPin, long prevDistance) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Convert to cm

  sensors_event_t a, g, temp; 
  // Get new sensor events 
  mpu.getEvent(&a, &g, &temp);

  // ignore over 100 cm
  return distance;
}

void readDistances() {
  long tmpDist = readDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT, distanceFront);
  distanceFront = tmpDist;
  delayMicroseconds(50);

  tmpDist = readDistance(TRIG_PIN_LEFT_45, ECHO_PIN_LEFT_45, distanceLeft45);
  distanceLeft45 = tmpDist;
  delayMicroseconds(50);

  tmpDist = readDistance(TRIG_PIN_LEFT, ECHO_PIN_LEFT, distanceLeft);
  distanceLeft = tmpDist;
}

long TranslateSpeedExp(long inValue) {
  switch (inValue) {
    case 3: return 255;
    case 2: return 245;
    case 1: return 235;
    case 0: return 0;
    case -1: return -235;
    case -2: return -245;
    case -3: return -255;
    default: return 123;
  }
}

void loop() {
  // read gyro speed
  calculateGyroSpeed();

  // resume drive speeds
  resumeMotors();

  // read current speed
  readDistances();

  // calculate drive speed expression: (-3 to +3)
  calculateMotorSpeed();

  // claculate steering epression (-3, to +3)
  calculateSteerDirection(cmdDrive);

  // grab previous state
  forwardSpeed = (prevDistanceFront - distanceFront);
  prevDistanceFront = distanceFront;
  prevDistance45 = distanceLeft45;
  prevDistanceLeft = distanceLeft;

  // output robot state
  outputState();

  // take a nap
  delayMicroseconds(50);
}
