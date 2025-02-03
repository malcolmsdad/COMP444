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
  // read and store front distance, wait 50 milliseconds to avoid interference
  _internalState.setFrontDistance(measureDistance(TRIG_PIN_FRONT, ECHO_PIN_FRONT));
  delayMicroseconds(50);

  // read and store left distance, and wait 50...
  _internalState.setLeftDistance(measureDistance(TRIG_PIN_LEFT, ECHO_PIN_LEFT));
  delayMicroseconds(50);

  // read and store left 45 distance
  _internalState.setFrontLeft45Distance(measureDistance(TRIG_PIN_LEFT_45, ECHO_PIN_LEFT_45));

  // write internal state to serial
  _internalState.SerialPrint();

  // what should the robot do?
  if (_internalState.IsCollisionDetected())
  {
    // Obstacle detected, back up and pause
    AvoidFrontObstacle();
  }
  else if (_internalState.IsLeftWallTooClose())
  {
    // Too close to wall, turn right slightly
    driveForward(DRIVE_SLOW);
    turnRightSharp();
  }
  else if (_internalState.IsLeftWallFollowing())
  {
    // Keep calm and keep driving
    driveForward(DRIVE_SLOW);

    // Proportional steering
    steerWheels(_internalState.ProportionalSteering());
  }
  else
  {
    // Maintain forward movement
    driveForward(DRIVE_FAST);
    turnLeftSlight();
  }

  // read the lux levels
  reportLightData();
}
