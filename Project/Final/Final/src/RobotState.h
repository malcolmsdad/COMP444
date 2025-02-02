#include <Arduino.h>

#ifndef RobotState_H
#define RobotState_H

class RobotState
{
public:
    // Constants
    const int MAX_DISTANCE = 80;
    const int MID_DISTANCE = 60;
    const int MIN_DISTANCE = 35;
    const int STOP_DISTANCE = 20;

    const int DRIVE_FAST = 235;
    const int DRIVE_SLOW = 230;
    const int DRIVE_COAST = 220;

    const int STEER_SLOW = 100;
    const int STEER_MID = 160;
    const int STEER_FAST = 240;

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

    int CalcIntegralDrive(int drive);

private:
    long FrontDistance = 0;
    long FrontLeft45Distance = 0;
    long LeftDistance = 0;

    long lastDistance1 = 0;
    long lastDistance2 = 0;
    long lastDistance3 = 0;

    int lastDriveValue = -1;
    int lastSteer = 0;

#endif // end of RobotState_H definition
};