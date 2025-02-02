

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
long distanceFront = 0;

long prevDistance45 = 0;
long distanceLeft45 = 0;

long prevDistanceLeft = 0;
long distanceLeft = 0;

long forwardSpeed = 0;
long steerDirection = 0;

// drive stats
long cmdDrive = 0;
long prevSpeed = 0;
long cmdSteer = 0;

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

  digitalWrite(STDBY_PIN, HIGH);  // Enable motor driver standby mode
  Serial.begin(9600);             // Initialize serial communication
}

long calculateMotorSpeed() {
  // current condition, going forward or back?
  if (prevSpeed < 0) {  // going backward.. so keep that direction a bit longer
    if (distanceFront < 30) return -3;
    if (distanceFront < 30) return -2;
    if (distanceFront < 50) return -1;
    if (distanceFront < 60) return 1;
  }

  // going forward
  if (distanceFront < 30) return -3;
  if (distanceFront < 30) return -1;
  if (distanceFront < 50) return 1;
  if (distanceFront < 75) return 2;
  return 3;
}

long prevSteer = 0;
long calculateSteerDirection(long direction) {
  if (direction < 0)
    return -3;

  // front obstacle!
  if (distanceFront <= 50) return 3;
  if (distanceFront <= 75) return 2;

  if (distanceLeft < 10) return 3;
  if (distanceLeft > 12) return -2;

  // do nothing
  return 0;
}


void driveMotor(long speed) {

  if (prevSpeed == speed) return;

  // turn motors off...
  digitalWrite(MTR_DRIVE_PIN1, LOW);
  digitalWrite(MTR_DRIVE_PIN2, LOW);

  if (prevSpeed != speed) {
    Serial.print(prevSpeed);
    Serial.print("->");
    Serial.print(speed);
  }
  // rev speed, take a quick break!
  if (prevSpeed * speed < 0) {
    Serial.print(" ** CHG DIR ** ");

    for (int i = 0; i < 10; i++)
      delayMicroseconds(15000);

    Serial.print("DONE ** ");
  }

  if (speed > MAX_SPEED) speed = MAX_SPEED;
  analogWrite(PWM_DRIVE_PIN, abs(speed));
  //analogWrite(PWM_DRIVE_PIN, 255);

  if (speed > 0) {
    digitalWrite(MTR_DRIVE_PIN1, HIGH);
  } else if (speed < 0) {
    digitalWrite(MTR_DRIVE_PIN2, HIGH);
  }

  if (prevSpeed != speed) {
    Serial.println();
  }

  prevSpeed = speed;
}

long prevDirection = 0;
void steerMotor(long direction) {

  digitalWrite(MTR_STEER_PIN1, LOW);
  digitalWrite(MTR_STEER_PIN2, LOW);

  if (direction > MAX_SPEED) direction = MAX_SPEED;

  analogWrite(PWM_STEER_PIN, 255);

  delayMicroseconds(direction/2);
  
  if (direction > 0)
    digitalWrite(MTR_STEER_PIN1, HIGH);
  else if (direction < 0)
    digitalWrite(MTR_STEER_PIN2, HIGH);

  prevDirection = direction;
}

void resumeMotors() {
  driveMotor(cmdDrive);
  steerMotor(cmdSteer);
}


long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;  // Convert to cm
  return distance;
}

void stopMotor() {
  analogWrite(PWM_DRIVE_PIN, MIN_SPEED);  // Stop the motor
  digitalWrite(MTR_DRIVE_PIN2, LOW);
}

void readDistances() {
  distanceFront = readDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT);
  delayMicroseconds(50);

  distanceLeft45 = readDistance(TRIG_PIN_LEFT_45, ECHO_PIN_LEFT_45);
  if (distanceLeft45 == 5) distanceLeft45 = prevDistance45;
  delayMicroseconds(50);

  distanceLeft = readDistance(TRIG_PIN_LEFT, ECHO_PIN_LEFT);
  if (distanceLeft == 5) distanceLeft = prevDistanceLeft;

  Serial.print("Fr:[");
  Serial.print(distanceFront);
  Serial.print(" cm] 45:[");
  Serial.print(distanceLeft45);
  Serial.print(" cm] Left:[");
  Serial.print(distanceLeft);
  Serial.print("] -> ");
}

long TranslateSpeedExp(long inValue) {
  int neg = (inValue / abs(inValue));
  switch (abs(inValue)) {
    case 3:
      return 255 * neg;
    case 2:
      return 245 * neg;
    case 1:
      return 235 * neg;
    case 0:
      return 0;
  }
}

void loop() {
  // resume drive speeds
  resumeMotors();

  // read current speed
  readDistances();

  // handle steering:
  //cmdSteer = (distanceFront > 40) ? 255 : (distanceFront > 20) ? 200 : (distanceFront > 10) ? 150 : -200;
  //cmdSteer = calculateMotorSpeed(REV_SPEED, MAX_SPEED / 4, DIST_LEFT_CLOSE, DIST_LEFT_FAR, distanceLeft45);
  //cmdSteer = calculateMotorSpeed(REV_SPEED, MAX_SPEED / 8, DIST_LEFT_CLOSE, DIST_FRONT_FAR, distanceFront);

  // handle throttle:
  long cmdDriveExp = calculateMotorSpeed();
  cmdDrive = TranslateSpeedExp(cmdDriveExp);

  long cmdSteerExp = calculateSteerDirection(cmdDrive);
  cmdSteer = TranslateSpeedExp(cmdSteerExp);

  //cmdDrive = calculateMotorSpeed(REV_SPEED, MAX_SPEED, DIST_LEFT_CLOSE, DIST_LEFT_FAR, distanceLeft45);
  //cmdDrive = calculateMotorSpeed(REV_SPEED, MAX_SPEED, DIST_LEFT_CLOSE, DIST_FRONT_FAR, distanceFront);

  Serial.print("Steer:");
  Serial.print(cmdSteer);
  Serial.print(" - Drive:");
  Serial.print(cmdDrive);

  // grab previous state
  prevDistanceFront = distanceFront;
  prevDistance45 = distanceLeft45;
  prevDistanceLeft = distanceLeft;

  // take a nap
  delayMicroseconds(250);
  Serial.println("[~]");
}
