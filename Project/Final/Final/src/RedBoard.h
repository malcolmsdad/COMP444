#include <Arduino.h>
#include <constants.h>
#include <RobotState.h>

#include <VirtualWire.h>
#include <ArduinoJson.h>

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
void transmitData(const char *msg);
void prepareDataPacket();
void reportLightData();