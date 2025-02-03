#include <Arduino.h>

#ifndef RobotState_H
#define RobotState_H

class RobotState
{
public:
    // Getter and Setters

    // Front Distance
    long getFrontDistance();
    void setFrontDistance(long inVal);

    // Front Left 45 Distance
    long getFrontLeft45Distance();
    void setFrontLeft45Distance(long inVal);

    // Left Distance
    long getLeftDistance();
    void setLeftDistance(long inVal);

    // last Distance Value
    void setLastDriveValue(int driveValue);
    int getLastDriveValue();

    // last steering value
    void setLastSteerValue(int steerValue);
    int getLastSteerValue();

    // store lux level
    void SetLuxLevel(int luxLevel);
    int GetLuxLevel();

    // calculate the integral control drive value
    int CalcIntegralDrive(int drive);

    // calculat proportional steering
    int ProportionalSteering();

    // write internal state to serial
    void SerialPrint();

    // We are about to collide!
    bool IsCollisionDetected();

    // The left wall is too close, move away
    bool IsLeftWallTooClose();

    // Actively find and follow a wall on the left
    bool IsLeftWallFollowing();

private:
    long FrontDistance = 0;         // distance front sensor to object
    long FrontLeft45Distance = 0;   // distance front left 45 degree to object
    long LeftDistance = 0;          // distance left sensor to wall

    long lastDistance1 = 0;         // previous distance
    long lastDistance2 = 0;         // distance from 2 cycles
    long lastDistance3 = 0;         // distance from 3 cycles

    int lastDriveValue = -1;        // last commanded drive value
    int lastSteer = 0;              // last commanded steering level

    int luxLevel = 0;               // current light level
}; 

#endif // end of RobotState_H definition
