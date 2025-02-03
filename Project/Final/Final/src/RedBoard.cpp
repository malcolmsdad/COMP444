#include <Arduino.h>
#include <constants.h>
#include "RedBoard.h"

RobotState _internalState;

void InitBoard()
{
    // set drive pins
    pinMode(PWM_DRIVE_PIN, OUTPUT);     // set pwm drive pin
    pinMode(MTR_DRIVE_PIN1, OUTPUT);    // set forward motion pin
    pinMode(MTR_DRIVE_PIN2, OUTPUT);    // set backward motion pin

    // set steering pins
    pinMode(PWM_STEER_PIN, OUTPUT);     // set steering pwm pin
    pinMode(MTR_STEER_PIN1, OUTPUT);    // set left steering pin
    pinMode(MTR_STEER_PIN2, OUTPUT);    // set right steering pin

    // motor control standby pin
    pinMode(STDBY_PIN, OUTPUT);         // set shared motr control standby pin

    // echo sensor pins
    pinMode(TRIG_PIN_FRONT, OUTPUT);    // set trigger for front sensor
    pinMode(ECHO_PIN_FRONT, INPUT);     // set echo pin for front sensor
    pinMode(TRIG_PIN_LEFT, OUTPUT);     // set trigger for left sensor
    pinMode(ECHO_PIN_LEFT, INPUT);      // set echo pin for left sensor
    pinMode(TRIG_PIN_LEFT_45, OUTPUT);  // set trigger for 45 left sensor
    pinMode(ECHO_PIN_LEFT_45, INPUT);   // set echo for 45 left sensor

    // Ensure standby is off to allow motor control
    digitalWrite(STDBY_PIN, HIGH);

    // setup RF pins and settings
    vw_set_tx_pin(RF_TX_PIN); // Set transmitter pin
    vw_set_rx_pin(RF_RX_PIN); // Set receiver pin
    vw_setup(RF_BAUD);        // Bits per second speed
    vw_rx_start();            // Start the receiver PLL running

    // Initialize serial communication
    Serial.begin(9600);
}

// Function to measure distance using an ultrasonic sensor
long measureDistance(int trigPin, int echoPin)
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
}

// drive the car backward by specified speed
void driveBackward(int pwmValue)
{
    Serial.print(" [driveBackward: ");
    digitalWrite(MTR_DRIVE_PIN1, LOW);
    digitalWrite(MTR_DRIVE_PIN2, HIGH);

    int currentDrive = _internalState.CalcIntegralDrive(pwmValue);
    analogWrite(PWM_DRIVE_PIN, currentDrive);
    Serial.print(currentDrive);
    Serial.print("] ");
    delayMicroseconds(currentDrive);
    _internalState.setLastDriveValue(pwmValue);
}

// Function to drive forward
void driveForward(int pwmValue)
{
    Serial.print(" [driveForward: ");
    digitalWrite(MTR_DRIVE_PIN1, HIGH);
    digitalWrite(MTR_DRIVE_PIN2, LOW);

    int currentDrive = _internalState.CalcIntegralDrive(pwmValue);
    analogWrite(PWM_DRIVE_PIN, currentDrive);
    Serial.print(currentDrive);
    Serial.print("] ");
    delayMicroseconds(currentDrive);
    _internalState.setLastDriveValue(pwmValue);
}

// Function to steer left with specified PWM value
void steerLeft(int pwm)
{
    Serial.print(" [steerLeft:");
    Serial.print(pwm);
    Serial.print("]");

    if (_internalState.getLastSteerValue() > 0)
    {
        digitalWrite(MTR_STEER_PIN1, LOW);
        digitalWrite(MTR_STEER_PIN2, HIGH);
    }
    analogWrite(PWM_STEER_PIN, pwm);
    delay(50);
}

// Function to steer right with specified PWM value
void steerRight(int pwm)
{
    Serial.print(" [steerRight:");
    Serial.print(pwm);
    Serial.print("]");

    if (_internalState.getLastSteerValue() < 0)
    {
        digitalWrite(MTR_STEER_PIN1, HIGH);
        digitalWrite(MTR_STEER_PIN2, LOW);
    }
    analogWrite(PWM_STEER_PIN, pwm);
    delay(50);
}

void steerWheels(int cmdSteer)
{

    // now steer wheels
    if (cmdSteer < 0)
        steerLeft(abs(cmdSteer));
    else
        steerRight(abs(cmdSteer));

    // store steer
    _internalState.setLastSteerValue(cmdSteer);
}

// Function to turn right sharply
void turnRightSharp()
{
    steerWheels(STEER_FAST);
}

// Function to turn left sharply
void turnLeftSharp()
{
    steerWheels(STEER_FAST * -1);
}

// Function to turn right slightly
void turnRightSlight()
{
    steerWheels(STEER_SLOW);
}

// Function to turn left slightly
void turnLeftSlight()
{
    steerWheels(STEER_SLOW * -1);
}

// Function to stop motors
void stopMotors()
{
    Serial.print(" [stopMotors] ");
    digitalWrite(MTR_DRIVE_PIN1, LOW);
    digitalWrite(MTR_DRIVE_PIN2, LOW);
    analogWrite(PWM_DRIVE_PIN, 0);
    delay(50);
}

// Function to back up for 2 seconds and pause for a second
void AvoidFrontObstacle()
{
    // stop motors
    stopMotors();

    // if no obstacles to the left, then turn wheels to right and backup
    if (_internalState.getLeftDistance() < MID_DISTANCE)
        turnRightSharp();
    else
        turnLeftSharp();

    // backup for a bit
    driveBackward(DRIVE_FAST);
    delay(500);
    driveBackward(DRIVE_SLOW);
    delay(150);

    // turn wheels back
    if (_internalState.getLeftDistance() < MID_DISTANCE)
        turnLeftSharp();
    else
        turnRightSharp();

    // now stop
    delay(50);
    stopMotors();
}

// read the light level to the internal state, and transmit via RF data
void reportLightData()
{
    // read ambient light level
    _internalState.SetLuxLevel(analogRead(LIGHT_SENSOR_PIN)); // Read the value from the light sensor

    // prepare data packet;
    PrepareAndTransmitLightData();
}

// transmit *msg data to RF 
void transmitData(const char *msg)
{
    // messageHistory.push_back(msg);  // Store the message in the vector
    Serial.print("TX:");
    Serial.print(msg);
    Serial.print("[");
    Serial.print(strlen(msg));
    Serial.print("]");

    // Create a new buffer with "1234" prepended
    char buffer[128];
    strcpy(buffer, "1234");
    strcat(buffer, msg);

    vw_send((uint8_t *)buffer, strlen(buffer));
    vw_wait_tx(); // Wait for the message to be sent
}

// read the light level from the internal state, encode it and transmit it via RF
void PrepareAndTransmitLightData()
{
    // Create a JSON document
    JsonDocument doc;
    doc["lux"] = _internalState.GetLuxLevel(); // Add the variable to the JSON document

    // Serialize the JSON document to a string
    char jsonBuffer[128];
    serializeJson(doc, jsonBuffer);

    transmitData(jsonBuffer);
}

