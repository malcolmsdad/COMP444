#include <Arduino.h>
#include <constants.h>
#include <RobotState.h>
#include "RedBoard.h"

void setup()
{
  InitBoard();
}

void loop()
{

  _internalState.setLastDriveValue(measureDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT));
  delayMicroseconds(50);

  _internalState.setLeftDistance(measureDistance(TRIG_PIN_LEFT, ECHO_PIN_LEFT));
  delayMicroseconds(50);

  _internalState.setFrontLeft45Distance(measureDistance(TRIG_PIN_LEFT_45, ECHO_PIN_LEFT_45));
  _internalState.SerialPrint();

  if (_internalState.IsCollisionDetected()) {
    // Obstacle detected, back up and pause
    AvoidFrontObstacle();
  } else if (_internalState.IsLeftWallTooClose()) {
    // Too close to wall, turn right slightly
    driveForward(DRIVE_SLOW);
    turnRightSharp();

  } else if (_internalState.IsLeftWallFollowing()) {
    // Keep calm and keep driving
    driveForward(DRIVE_SLOW);

    // Proportional steering
    steerWheels(_internalState.ProportionalSteering());

  } else {
    // Maintain forward movement
    driveForward(DRIVE_FAST);
    turnLeftSlight();
  }
}
