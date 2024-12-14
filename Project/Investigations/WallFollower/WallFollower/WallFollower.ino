#define PWM_PIN 11
#define INPUT_PIN 13
#define STDBY_PIN 12
#define TRIG_PIN_FRONT 6
#define ECHO_PIN_FRONT 7

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
  
  digitalWrite(STDBY_PIN, HIGH); // Enable motor driver standby mode
  Serial.begin(9600); // Initialize serial communication
}

long readDistance() {
  digitalWrite(TRIG_PIN_FRONT, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN_FRONT, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN_FRONT, LOW);

  long duration = pulseIn(ECHO_PIN_FRONT, HIGH);
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
  long distanceFront = readDistance();
  Serial.print("Front Distance: ");
  Serial.print(distanceFront);
  Serial.println(" cm");

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

  // Set motor direction
  digitalWrite(INPUT_PIN, HIGH);

  delay(50); // Reduce delay for more frequent distance checks
}
