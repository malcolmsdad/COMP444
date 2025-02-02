#ifndef CONSTANTH_H
#define CONSTANTS_H

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

#endif

// Constants
extern const int MAX_DISTANCE;
extern const int MID_DISTANCE;
extern const int MIN_DISTANCE;
extern const int STOP_DISTANCE;

extern const int DRIVE_FAST;
extern const int DRIVE_SLOW;
extern const int DRIVE_COAST;

extern const int STEER_SLOW;
extern const int STEER_MID;
extern const int STEER_FAST;

extern const float bias; 
