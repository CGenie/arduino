// Class that makes easier calculation of accel/gyro readings for MPU6050

#include <math.h>

#include <Arduino.h>

// MPU6050-specific
#include <I2Cdev.h>
#include <MPU6050.h>

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

#include "Integrator.h"

#ifndef _ACCELGYRO_H
#define _ACCELGYRO_H

class AccelGyro;
class AccelGyroKalman;
class Calibrator;

class Calibrator {
    private:
        int16_t left, right;
        char axis;
        char type;
        double target;

        void next(double val);
        void setOffset(AccelGyro* ag);

    public:
        Calibrator(char type, char axis, double target);
        ~Calibrator();

        void calibrate(AccelGyro* ag, double val);
        double error(double val);
        void leftRightReset();
};


class AccelGyro {
    private:
        MPU6050* accelgyro;
        static const int MAXACC = 32768 - 1;
        static const int MAXGYR = 32768 - 1;
        static const double GYRO_DEGREES_PER_SEC = 250;

        double calibration_error(double ax, double ay, double az, double gx, double gy, double gz);

    public:
        AccelGyro();
        ~AccelGyro();

        double axangle;
        double ayangle;
        double azangle;
        Integrator *gxint;
        Integrator *gyint;
        Integrator *gzint;
        double agxint;

        AccelGyroKalman *kalman;

        void setup();
        void calibrate();
        void compute();
        void setOffset(char type, char axis, int16_t offset);
        void setAccOffset(char axis, int16_t offset);
        void setGyrOffset(char axis, int16_t offset);
        void getMotion6Normalized(double* ax, double* ay, double* az, double* gx, double* gy, double* gz);
        void getMotion6AvgNormalized(double* ax, double* ay, double* az, double* gx, double* gy, double* gz);

        // Serial.print compatible with arduino-pyplot
        void plotMotion6Normalized();
        void plot(int size, ...);
};


class AccelGyroKalman {
    private:
        //double gyroAngleRate;    // gyro angle rate reading
        //double accAngle;         // accel angle reading
        double y;                // innovation
        double P[2][2];          // error covariance matrix
        double K[2];             // Kalman gain
        double S;                // innovation covariance
        unsigned long last_time; // last time step

        static const double Q_angle = 0.001;  // angle gyro measurement variance
        static const double Q_bias = 0.003;   // gyro bias variance
        static const double R_angle = 0.03;   // angle acceleration
                                              // measurement variance
    public:
        AccelGyroKalman();
        ~AccelGyroKalman();

        double angle;               // filtered angle
        double angleRate;           // rate of angle change
        Integrator *angleIntegral;  // angle integral
        double bias;                // filtered bias
        double biasRate;            // rate of bias change

        void compute(double accAngle, double gyroAngleRate);
        void reset();
};

#endif  // _ACCELGYRO_H
