#define PWM_PIN 11
#define INPUT_PIN 13
#define STDBY_PIN 12
#define TRIG_PIN_FRONT 2
#define ECHO_PIN_FRONT 3
#define TRIG_PIN_LEFT 4
#define ECHO_PIN_LEFT 5
#define TRIG_PIN_LEFT_45 0
#define ECHO_PIN_LEFT_45 1

const int MAX_SPEED = 255; // PWM value for full speed
const int MIN_SPEED = 0; // PWM value for stop
const int BRAKE_THRESHOLD = 10; // Distance in cm to start active braking
const int STOP_THRESHOLD = 5; // Distance in cm to stop the motor completely
const int BRAKE_DURATION = 300; // Duration of the active braking phase in milliseconds
const int REVERSE_DURATION = 200; // Duration for the reverse motion in milliseconds

void setup() {
  pinMode(PWM_PIN, OUTPUT);
  pinMode(INPUT_PIN, OUTPUT);
  pinMode(STDBY_PIN, OUTPUT);
  pinMode(TRIG_PIN_FRONT, OUTPUT);
  pinMode(ECHO_PIN_FRONT, INPUT);
  pinMode(TRIG_PIN_LEFT, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  pinMode(TRIG_PIN_LEFT_45, OUTPUT);
  pinMode(ECHO_PIN_LEFT_45, INPUT);
  
  digitalWrite(STDBY_PIN, HIGH); // Enable motor driver standby mode
  Serial.begin(9600); // Initialize serial communication
}

long readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

void stopMotor() {
  // Apply aggressive braking by reversing the motor briefly
  digitalWrite(INPUT_PIN, LOW);
  analogWrite(PWM_PIN, MAX_SPEED);
  delay(REVERSE_DURATION);
  analogWrite(PWM_PIN, MIN_SPEED); // Stop the motor
}

void loop() {
  long distanceFront = readDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT);
  long distanceLeft = readDistance(TRIG_PIN_LEFT, ECHO_PIN_LEFT);
  long distanceLeft45 = readDistance(TRIG_PIN_LEFT_45, ECHO_PIN_LEFT_45);

  Serial.print("Front Distance: ");
  Serial.print(distanceFront);
  Serial.print(" cm, Left Distance: ");
  Serial.print(distanceLeft);
  Serial.print(" cm, Left 45 Degree Distance: ");
  Serial.println(distanceLeft45);

  int speed;
  if (distanceFront < STOP_THRESHOLD) { // Closer than 2 inches (approximately 5 cm)
    stopMotor(); // Stop the motor using aggressive braking
    while (true); // Infinite loop to stop the program
  } else if (distanceFront < BRAKE_THRESHOLD) { // Within braking threshold
    // Apply active braking to slow down more effectively
    digitalWrite(INPUT_PIN, LOW);
    analogWrite(PWM_PIN, MAX_SPEED);
    delay(BRAKE_DURATION); // Extended braking duration
    speed = MIN_SPEED; // Prepare to stop
  } else if (distanceFront > 91) { // Over 36 inches
    speed = MAX_SPEED; // Full speed
  } else {
    // Map the distance range (10 cm to 91 cm) to speed range (MIN_SPEED to MAX_SPEED)
    speed = map(distanceFront, 10, 91, MIN_SPEED, MAX_SPEED);
  }

  // Set motor speed based on mapped value
  analogWrite(PWM_PIN, speed);

  // Wall following logic
  if (distanceLeft < 15 || distanceLeft45 < 15) { // Less than 6 inches
    // Turn right
    digitalWrite(INPUT_PIN, LOW);
    analogWrite(PWM_PIN, speed);
  } else if (distanceLeft > 18 && distanceLeft45 > 18) { // More than 7 inches
    // Turn left
    digitalWrite(INPUT_PIN, HIGH);
    analogWrite(PWM_PIN, speed);
  } else {
    // Drive straight
    digitalWrite(INPUT_PIN, HIGH);
    analogWrite(PWM_PIN, speed);
  }

  delay(50); // Reduce delay for more frequent distance checks
}
