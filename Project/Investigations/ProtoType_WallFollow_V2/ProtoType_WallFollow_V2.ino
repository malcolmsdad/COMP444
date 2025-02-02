#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Ultrasonic.h>

Adafruit_MPU6050 mpu;

// Constants
#define PWM_DRIVE_PIN 5
#define MTR_DRIVE_PIN1 6
#define MTR_DRIVE_PIN2 7

#define PWM_STEER_PIN 3
#define MTR_STEER_PIN1 2
#define MTR_STEER_PIN2 1

#define STDBY_PIN 4

#define TRIG_PIN_FRONT 9
#define ECHO_PIN_FRONT 8
#define TRIG_PIN_LEFT 13
#define ECHO_PIN_LEFT 12
#define TRIG_PIN_LEFT_45 11
#define ECHO_PIN_LEFT_45 10

const int RNG_FRONT_LMT = 30; // Distance limit in cm
const int RNG_LEFT_LMT = 20;  // Distance to the left wall in cm

const float TARGET_FWD_G = 2.0;
const float TARGET_BACK_G = -2.0;

// Variables
float CommandDrive = 0;
float CommandSteer = 0;
float GTracked = 0;
float GCurrent = 0;
float accelOffsets[3] = {0, 0, 0};
float gyroOffsets[3] = {0, 0, 0};
Ultrasonic rngFront(TRIG_PIN_FRONT, ECHO_PIN_FRONT);
Ultrasonic rngLeft(TRIG_PIN_LEFT, ECHO_PIN_LEFT);

void setup() {
    Serial.begin(9600);
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
            delay(10);
        }
    }

    pinMode(PWM_DRIVE_PIN, OUTPUT);
    pinMode(MTR_DRIVE_PIN1, OUTPUT);
    pinMode(MTR_DRIVE_PIN2, OUTPUT);

    pinMode(PWM_STEER_PIN, OUTPUT);
    pinMode(MTR_STEER_PIN1, OUTPUT);
    pinMode(MTR_STEER_PIN2, OUTPUT);

    pinMode(STDBY_PIN, OUTPUT);
    digitalWrite(STDBY_PIN, HIGH); // Enable motors

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    Serial.println("MPU6050 and Ultrasonic sensors initialized");
    calibrateMPU6050();
}

void loop() {
    readAllSensors();
    if (checkFrontCollision()) {
        if (rngFront.read() <= RNG_FRONT_LMT / 2) {
            avoidObstacle();
        } else {
            motorSteerRight();
            driveForward();
        }
    } else if (checkFrontLeftCollision()) {
        motorSteerRight();
        driveForward();
    } else {
        followLeftWall();
    }
}

void readAllSensors() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    GCurrent = sqrt((a.acceleration.x - accelOffsets[0]) * (a.acceleration.x - accelOffsets[0]) +
                    (a.acceleration.y - accelOffsets[1]) * (a.acceleration.y - accelOffsets[1]) +
                    (a.acceleration.z - accelOffsets[2]) * (a.acceleration.z - accelOffsets[2]));

    Serial.print("Acceleration: ");
    Serial.println(GCurrent);

    int distanceFront = rngFront.read();
    int distanceLeft = rngLeft.read();

    Serial.print("Distance Front: ");
    Serial.print(distanceFront);
    Serial.print(" cm, Distance Left: ");
    Serial.print(distanceLeft);
    Serial.println(" cm");
}

void calibrateMPU6050() {
    const int calibration_samples = 1000;
    float accelSum[3] = {0, 0, 0};
    float gyroSum[3] = {0, 0, 0};

    for (int i = 0; i < calibration_samples; i++) {
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);

        accelSum[0] += a.acceleration.x;
        accelSum[1] += a.acceleration.y;
        accelSum[2] += a.acceleration.z;

        gyroSum[0] += g.gyro.x;
        gyroSum[1] += g.gyro.y;
        gyroSum[2] += g.gyro.z;

        delay(5); // Small delay between samples
    }

    accelOffsets[0] = accelSum[0] / calibration_samples;
    accelOffsets[1] = accelSum[1] / calibration_samples;
    accelOffsets[2] = accelSum[2] / calibration_samples;

    gyroOffsets[0] = gyroSum[0] / calibration_samples;
    gyroOffsets[1] = gyroSum[1] / calibration_samples;
    gyroOffsets[2] = gyroSum[2] / calibration_samples;

    Serial.println("MPU6050 calibrated");
}

bool checkFrontCollision() {
    int distanceFront = rngFront.read();
    Serial.print("Checking Front Collision: ");
    Serial.println(distanceFront <= RNG_FRONT_LMT ? "Detected" : "Not Detected");
    return distanceFront <= RNG_FRONT_LMT;
}

bool checkFrontLeftCollision() {
    int distanceFrontLeft = rngLeft.read();
    Serial.print("Checking Front-Left Collision: ");
    Serial.println(distanceFrontLeft <= RNG_FRONT_LMT ? "Detected" : "Not Detected");
    return distanceFrontLeft <= RNG_FRONT_LMT;
}

void followLeftWall() {
    int distanceLeft = rngLeft.read();
    Serial.print("Following Left Wall. Distance Left: ");
    Serial.println(distanceLeft);

    if (distanceLeft > RNG_LEFT_LMT) {
        motorSteerLeft();
        driveForward();
    } else if (distanceLeft < RNG_LEFT_LMT) {
        motorSteerRight();
        driveForward();
    } else {
        driveForward();
    }
}

void avoidObstacle() {
    Serial.println("Avoiding Obstacle");
    motorStop();
    delay(1000);
    driveBackward();
    delay(1000);
    motorSteerRight();
    driveForward();
    delay(2000); // Drive right for a while to clear the obstacle
}

void motorStop() {
    Serial.println("Stopping Motors");
    adjustGForce(1.0);
    adjustGForce(-1.0);
    adjustGForce(0.5);
    adjustGForce(-0.5);

    digitalWrite(PWM_DRIVE_PIN, LOW);
    digitalWrite(MTR_DRIVE_PIN1, LOW);
    digitalWrite(MTR_DRIVE_PIN2, LOW);

    calibrateMPU6050(); // Recalibrate the gyro sensor
    Serial.println("Motors stopped and sensor recalibrated");
}

void driveForward() {
    Serial.println("Driving Forward");
    GTracked = 0;
    int motorSpeed = 230; // Start with a higher initial speed
    while (GTracked < TARGET_FWD_G) {
        motorSpeed = min(motorSpeed + 12, 255); // Increase power faster
        analogWrite(PWM_DRIVE_PIN, motorSpeed);
        digitalWrite(MTR_DRIVE_PIN1, HIGH);
        digitalWrite(MTR_DRIVE_PIN2, LOW);

        readAllSensors();
        GTracked += GCurrent;

        Serial.print("Driving Forward. GTracked: ");
        Serial.println(GTracked);
    }
}

void driveBackward() {
    Serial.println("Reversing");
    GTracked = 0;
    int motorSpeed = 230; // Start with a higher initial speed
    while (GTracked > TARGET_BACK_G) {
        motorSpeed = min(motorSpeed + 12, 255); // Increase power faster
        analogWrite(PWM_DRIVE_PIN, motorSpeed);
        digitalWrite(MTR_DRIVE_PIN1, LOW);
        digitalWrite(MTR_DRIVE_PIN2, HIGH);

        readAllSensors();
        GTracked += GCurrent;

        Serial.print("Reversing. GTracked: ");
        Serial.println(GTracked);
    }
}

void motorSteerLeft() {
    analogWrite(PWM_STEER_PIN, 255);
    digitalWrite(MTR_STEER_PIN1, HIGH);
    digitalWrite(MTR_STEER_PIN2, LOW);
    Serial.println("Steering Left");
}

void motorSteerRight() {
    analogWrite(PWM_STEER_PIN, 255);
    digitalWrite(MTR_STEER_PIN1, LOW);
    digitalWrite(MTR_STEER_PIN2, HIGH);
    Serial.println("Steering Right");
}

void adjustGForce(float targetG) {
    GTracked = 0;
    int motorSpeed = 255;
    while (abs(GTracked) < abs(targetG)) {
        analogWrite(PWM_DRIVE_PIN, motorSpeed);
        if (targetG > 0) {
            digitalWrite(MTR_DRIVE_PIN1, HIGH);
            digitalWrite(MTR_DRIVE_PIN2, LOW);
        } else {
            digitalWrite(MTR_DRIVE_PIN1, LOW);
            digitalWrite(MTR_DRIVE_PIN2, HIGH);
        }

        readAllSensors();
        GTracked += GCurrent;

        Serial.print("Adjusting GForce. Target: ");
        Serial.print(targetG);
        Serial.print(", GTracked: ");
        Serial.println(GTracked);

        motorSpeed = max(motorSpeed - 5, 0);
    }
    digitalWrite(PWM_DRIVE_PIN, LOW);
}