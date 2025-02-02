#include <Arduino.h>
#include <constants.h>
#include <RobotState.h>

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

extern RobotState _internalState;

void InitBoard();
long measureDistance(int trigPin, int echoPin);
void driveBackward(int pwmValue);
void driveForward(int pwmValue);
void steerLeft(int pwm);
void steerRight(int pwm);
void steerWheels(int cmdSteer);
void turnRightSharp();
void turnLeftSharp();
void turnRightSlight();
void turnLeftSlight();
void stopMotors();
void AvoidFrontObstacle();