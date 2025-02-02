#include "RobotState.h"

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


int RobotState::CalcIntegralDrive(int drive)
{
    int distOver3 = abs(lastDistance3 - lastDistance2) + abs(lastDistance2 - lastDistance1);
    if (distOver3 > 5)
        return drive;
    else
        return DRIVE_FAST;
}
