// Class that makes easier calculation of accel/gyro readings for MPU6050

#include <math.h>

#include <Arduino.h>

// MPU6050-specific
#include <I2Cdev.h>
#include <MPU6050.h>

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

#ifndef _ACCELGYRO_H
#define _ACCELGYRO_H

class AccelGyro;
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
};

class AccelGyro {
    private:
        MPU6050* accelgyro;
        static const int MAXACC = 32768 - 1;
        static const int MAXGYR = 32768 - 1;

        double calibration_error(double ax, double ay, double az, double gx, double gy, double gz);

    public:
        AccelGyro();
        ~AccelGyro();
        
        void setup();
        void calibrate();
        void setOffset(char type, char axis, int16_t offset);
        void setAccOffset(char axis, int16_t offset);
        void setGyrOffset(char axis, int16_t offset);
        void getMotion6Normalized(double* ax, double* ay, double* az, double* gx, double* gy, double* gz);
        void getMotion6AvgNormalized(double* ax, double* ay, double* az, double* gx, double* gy, double* gz);
};

#endif  // _ACCELGYRO_H
