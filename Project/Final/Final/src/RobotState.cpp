#include "RobotState.h"
#include <Arduino.h>
#include <constants.h>

// Getter and Setters

// Front Distance
long RobotState::getFrontDistance() { return FrontDistance; }
void RobotState::setFrontDistance(long inVal)
{
    lastDistance3 = lastDistance2;
    lastDistance2 = lastDistance1;
    lastDistance1 = FrontDistance;
    FrontDistance = inVal;
}

// Front Left 45 Distance
long RobotState::getFrontLeft45Distance() { return FrontLeft45Distance; }
void RobotState::setFrontLeft45Distance(long inVal)
{
    FrontLeft45Distance = inVal;
}

// Left Distance
long RobotState::getLeftDistance() { return LeftDistance; }
void RobotState::setLeftDistance(long inVal)
{
    LeftDistance = inVal;
}

void RobotState::setLastDriveValue(int driveValue) { lastDriveValue = driveValue; }
int RobotState::getLastDriveValue()
{
    return lastDriveValue;
}

void RobotState::setLastSteerValue(int steerValue) { lastSteer = steerValue; }
int RobotState::getLastSteerValue()
{
    return lastSteer;
}

int RobotState::CalcIntegralDrive(int drive)
{
    int distOver3 = abs(lastDistance3 - lastDistance2) + abs(lastDistance2 - lastDistance1);
    if (distOver3 > 5)
        return drive;
    else
        return DRIVE_FAST;
}

// Proportional steering control function
int RobotState::ProportionalSteering()
{
    int error = LeftDistance - STOP_DISTANCE;

    if (error <= -5)
    {
        return STEER_FAST; // Full right steer
    }
    else if (error >= 5)
    {
        return -1 * STEER_FAST; // Full left steer
    }
    else
    {
        int steerCmd = constrain(bias * error, -1 * STEER_FAST, STEER_FAST);
        return steerCmd;
    }
}

void RobotState::SerialPrint()
{
    Serial.print(" [Front: ");
    Serial.print(FrontDistance);
    Serial.print(" cm, Left: ");
    Serial.print(LeftDistance);
    Serial.print(" cm, Left45: ");
    Serial.print(FrontLeft45Distance);
    Serial.println(" cm] ");
}

bool RobotState::IsCollisionDetected() { return (FrontDistance < STOP_DISTANCE || FrontLeft45Distance < STOP_DISTANCE); }
bool RobotState::IsLeftWallTooClose() { return (FrontDistance < MIN_DISTANCE || FrontLeft45Distance < MIN_DISTANCE); }
bool RobotState::IsLeftWallFollowing() { return (LeftDistance < MID_DISTANCE); }