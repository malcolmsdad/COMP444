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

    void setLastSteerValue(int steerValue);
    int getLastSteerValue();

    void SetLuxLevel(int luxLevel);
    int GetLuxLevel();

    int CalcIntegralDrive(int drive);
    int ProportionalSteering();

    void SerialPrint();

    bool IsCollisionDetected();
    bool IsLeftWallTooClose();
    bool IsLeftWallFollowing();

private:
    long FrontDistance = 0;
    long FrontLeft45Distance = 0;
    long LeftDistance = 0;

    long lastDistance1 = 0;
    long lastDistance2 = 0;
    long lastDistance3 = 0;

    int lastDriveValue = -1;
    int lastSteer = 0;

    int luxLevel = 0;
}; // Add the semicolon here

#endif // end of RobotState_H definition
