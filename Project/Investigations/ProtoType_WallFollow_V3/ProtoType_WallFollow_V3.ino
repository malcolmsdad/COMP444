// Define Pins
#define PWM_DRIVE_PIN 5
#define MTR_DRIVE_PIN1 6
#define MTR_DRIVE_PIN2 7

#define PWM_STEER_PIN 3
#define MTR_STEER_PIN1 1
#define MTR_STEER_PIN2 2

#define STDBY_PIN 4

#define TRIG_PIN_FRONT 9
#define ECHO_PIN_FRONT 8
#define TRIG_PIN_LEFT 13
#define ECHO_PIN_LEFT 12
#define TRIG_PIN_LEFT_45 11
#define ECHO_PIN_LEFT_45 10

// Constants
const int MAX_DISTANCE = 80;
const int MID_DISTANCE = 60;
const int MIN_DISTANCE = 35;
const int STOP_DISTANCE = 20;

const int DRIVE_FAST = 235;
const int DRIVE_SLOW = 230;
const int DRIVE_COAST = 220;

const int STEER_SLOW = 100;
const int STEER_MID = 160;
const int STEER_FAST = 200;

const float bias = 10.0;

long lastDistance1 = 0;
long lastDistance2 = 0;
long lastDistance3 = 0;

int lastDriveValue = -1;
int lastSteer = 0;

// Function to set up pins
void setup() {
  pinMode(PWM_DRIVE_PIN, OUTPUT);
  pinMode(MTR_DRIVE_PIN1, OUTPUT);
  pinMode(MTR_DRIVE_PIN2, OUTPUT);

  pinMode(PWM_STEER_PIN, OUTPUT);
  pinMode(MTR_STEER_PIN1, OUTPUT);
  pinMode(MTR_STEER_PIN2, OUTPUT);

  pinMode(STDBY_PIN, OUTPUT);

  pinMode(TRIG_PIN_FRONT, OUTPUT);
  pinMode(ECHO_PIN_FRONT, INPUT);
  pinMode(TRIG_PIN_LEFT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  pinMode(TRIG_PIN_LEFT_45, OUTPUT);
  pinMode(ECHO_PIN_LEFT_45, INPUT);

  // Ensure standby is off to allow motor control
  digitalWrite(STDBY_PIN, HIGH);

  // Initialize serial communication
  Serial.begin(9600);
}

// Function to measure distance using an ultrasonic sensor
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

int calcDrive(int drive) {
  int distOver3 = abs(lastDistance3 - lastDistance2) + abs(lastDistance2 - lastDistance1);
  if (distOver3 > 5)
    return drive;
  else
    return DRIVE_FAST;
}

void driveBackward(int pwmValue) {
  Serial.print(" [driveBackward: ");
  digitalWrite(MTR_DRIVE_PIN1, LOW);
  digitalWrite(MTR_DRIVE_PIN2, HIGH);

  int currentDrive = calcDrive(pwmValue);
  analogWrite(PWM_DRIVE_PIN, currentDrive);
  Serial.print(currentDrive);
  Serial.print("] ");
  delayMicroseconds(currentDrive);
  lastDriveValue = pwmValue;
}

// Function to drive forward
void driveForward(int pwmValue) {
  Serial.print(" [driveForward: ");
  digitalWrite(MTR_DRIVE_PIN1, HIGH);
  digitalWrite(MTR_DRIVE_PIN2, LOW);

  int currentDrive = calcDrive(pwmValue);
  analogWrite(PWM_DRIVE_PIN, currentDrive);
  Serial.print(currentDrive);
  Serial.print("] ");
  delayMicroseconds(currentDrive);
  lastDriveValue = pwmValue;
}

// Function to steer left with specified PWM value
void steerLeft(int pwm) {
  Serial.print(" [steerLeft:");
  Serial.print(pwm);
  Serial.print("]");

  if (lastSteer > 0) {
    digitalWrite(MTR_STEER_PIN1, LOW);
    digitalWrite(MTR_STEER_PIN2, HIGH);
  }
  analogWrite(PWM_STEER_PIN, pwm);
  delay(50);
}

// Function to steer right with specified PWM value
void steerRight(int pwm) {
  Serial.print(" [steerRight:");
  Serial.print(pwm);
  Serial.print("]");

  if (lastSteer < 0) {
    digitalWrite(MTR_STEER_PIN1, HIGH);
    digitalWrite(MTR_STEER_PIN2, LOW);
  }
  analogWrite(PWM_STEER_PIN, pwm);
  delay(50);
}

void steerWheels(int cmdSteer) {

  // now steer wheels
  if (cmdSteer < 0)
    steerLeft(abs(cmdSteer));
  else
    steerRight(abs(cmdSteer));

  // store steer
  lastSteer = cmdSteer;
}

// Proportional steering control function
int proportionalSteering(int distanceLeft) {
  int error = distanceLeft - STOP_DISTANCE;

  if (error <= -5) {
    return STEER_FAST;  // Full right steer
  } else if (error >= 5) {
    return -1 * STEER_FAST;  // Full left steer
  } else {
    int steerCmd = constrain(bias * error, -1 * STEER_FAST, STEER_FAST);
    return steerCmd;
  }
}

// Function to turn right sharply
void turnRightSharp() {
  steerWheels(STEER_FAST);
}

// Function to turn left sharply
void turnLeftSharp() {
  steerWheels(STEER_FAST * -1);
}

// Function to turn right slightly
void turnRightSlight() {
  steerWheels(STEER_SLOW);
}

// Function to turn left slightly
void turnLeftSlight() {
  steerWheels(STEER_SLOW * -1);
}

// Function to stop motors
void stopMotors() {
  Serial.print(" [stopMotors] ");
  digitalWrite(MTR_DRIVE_PIN1, LOW);
  digitalWrite(MTR_DRIVE_PIN2, LOW);
  analogWrite(PWM_DRIVE_PIN, 0);
  delay(50);
}

// Function to back up for 2 seconds and pause for a second
void AvoidFrontObstacle(int distanceLeft) {

  // stop motors
  stopMotors();

  // if no obstacles to the left, then turn wheels to right and backup
  turnLeftSharp();
  turnRightSharp();
  turnLeftSlight();
  turnRightSlight();
  turnLeftSharp();


  // backup for a bit
  driveBackward(DRIVE_FAST);
  delay(750);
  turnLeftSharp();

  driveBackward(DRIVE_SLOW);
  delay(300);

  // turn wheels back
  turnRightSharp();

  // now stop
  delay(150);
  stopMotors();
}

void loop() {
  long distanceFront = measureDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT);
  lastDistance3 = lastDistance2;
  lastDistance2 = lastDistance1;
  lastDistance1 = distanceFront;
  delayMicroseconds(50);

  long distanceLeft = measureDistance(TRIG_PIN_LEFT, ECHO_PIN_LEFT);
  delayMicroseconds(50);
  long distanceLeft45 = measureDistance(TRIG_PIN_LEFT_45, ECHO_PIN_LEFT_45);

  Serial.print(" [Front: ");
  Serial.print(distanceFront);
  Serial.print(" cm, Left: ");
  Serial.print(distanceLeft);
  Serial.print(" cm, Left45: ");
  Serial.print(distanceLeft45);
  Serial.println(" cm] ");

  if (distanceFront < STOP_DISTANCE || distanceLeft45 < STOP_DISTANCE) {
    // Obstacle detected, back up and pause
    AvoidFrontObstacle(distanceLeft);
  } else if (distanceFront < MIN_DISTANCE || distanceLeft45 < MIN_DISTANCE+5) {

    // Too close to wall, turn right slightly
    driveForward(DRIVE_SLOW);
    turnRightSharp();

  } else if (distanceLeft < MID_DISTANCE) {

    // Keep calm and keep driving
    driveForward(DRIVE_SLOW);

    // Proportional steering
    int steerCommand = proportionalSteering(distanceLeft);
    steerWheels(steerCommand);

  } else {
    // Maintain forward movement
    driveForward(DRIVE_FAST);
    turnLeftSlight();
  }
}
