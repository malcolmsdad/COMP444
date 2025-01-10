// Define Pins
#define PWM_DRIVE_PIN 5
#define MTR_DRIVE_PIN1 6
#define MTR_DRIVE_PIN2 7

#define PWM_STEER_PIN 3
#define MTR_STEER_PIN1 2
#define MTR_STEER_PIN2 1

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

const int DRIVE_FAST = 255;
const int DRIVE_SLOW = 150;
const int DRIVE_COAST = 90;

const int STEER_SLOW = 220;
const int STEER_FAST = 245;

long lastDistance1 = 0;
long lastDistance2 = 0;
long lastDistance3 = 0;




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

int lastDriveValue = -1;

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

String lastTurn = "";

int calcSteer(String currentTurn, int pwm) {
  if (currentTurn == lastTurn)
    return pwm;

  lastTurn = currentTurn;
  return STEER_FAST;
}

// Function to turn right sharply
void turnRightSharp() {

  Serial.print(" [turnRightSharp] ");
  digitalWrite(MTR_STEER_PIN1, HIGH);
  digitalWrite(MTR_STEER_PIN2, LOW);
  analogWrite(PWM_STEER_PIN, calcSteer("turnRightSharp", STEER_FAST));
  delay(STEER_FAST);
}

// Function to turn left sharply
void turnLeftSharp() {

  Serial.print(" [turnLeftSharp] ");
  digitalWrite(MTR_STEER_PIN1, LOW);
  digitalWrite(MTR_STEER_PIN2, HIGH);
  analogWrite(PWM_STEER_PIN, calcSteer("turnLeftSharp", STEER_FAST));
  delay(STEER_FAST);
}

// Function to turn right slightly
void turnRightSlight() {
  Serial.print(" [turnRightSlight] ");

  digitalWrite(MTR_STEER_PIN1, HIGH);
  digitalWrite(MTR_STEER_PIN2, LOW);
  analogWrite(PWM_STEER_PIN, calcSteer("turnRightSlight", STEER_SLOW));
  delay(STEER_SLOW);
}

// Function to turn left slightly
void turnLeftSlight() {
  Serial.print(" [turnLeftSlight] ");

  digitalWrite(MTR_STEER_PIN1, LOW);
  digitalWrite(MTR_STEER_PIN2, HIGH);
  analogWrite(PWM_STEER_PIN, calcSteer("turnLeftSlight", STEER_SLOW));
  delay(STEER_SLOW);
}

// Function to stop motors
void stopMotors() {
  Serial.print(" [stopMotors] ");

  digitalWrite(MTR_DRIVE_PIN1, LOW);
  digitalWrite(MTR_DRIVE_PIN2, LOW);
  analogWrite(PWM_DRIVE_PIN, 0);
}

// Function to back up for 2 seconds and pause for a second
void backupAndPause() {
  Serial.print("[BackupAndPause] ");
  turnLeftSharp();
  digitalWrite(MTR_DRIVE_PIN1, LOW);
  digitalWrite(MTR_DRIVE_PIN2, HIGH);
  analogWrite(PWM_DRIVE_PIN, DRIVE_FAST);  // Full speed backward
  delay(2000);                             // Back up for 2 seconds
  stopMotors();                            // Stop motors
  delayMicroseconds(100);                  // Pause for 1 second
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
    backupAndPause();
  } else if (distanceFront < MIN_DISTANCE || distanceLeft45 < MIN_DISTANCE) {

    // Too close to wall, turn right slightly
    turnRightSharp();
    driveForward(DRIVE_COAST);
  } else if (distanceLeft < MAX_DISTANCE) {
    // Too far from wall, turn left slightly
    turnLeftSlight();
    driveForward(DRIVE_COAST);
  } else {
    // Maintain forward movement
    driveForward(DRIVE_SLOW);
  }
}
