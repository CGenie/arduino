// Some data taken from
// http://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/step4/One-Motor/

#include <Arduino.h>

#include "BoolPin.h"

#ifndef _MOTOR_H
#define _MOTOR_H

class Motor {
    private:
        char variant;  // 'A' or 'B'
        bool reversed;

        BoolPin* directionPin;
        BoolPin* brakePin;
        BoolPin* encoderPin;
        int analogSpeedPin;
        long encoderCount;
        double targetAngle;

        void init(char variant, bool reversed);  // constructor

    public:
        Motor(char variant);
        Motor(char variant, bool reversed);
        ~Motor();

        int currentSpeed;
        int shaftRatio;
        
        void setup();

        double getEncoderAngle();  // in radians
        bool getCurrentSpeedDirection();
        void setSpeed(int speed);

        // generic function for setting motor speed & direction
        // speed > 0 => forward, speed < 0 => backward
        void go(int speed);

        // main encoder loop, call this function often to get good
        // results
        void computeEncoder();

        void turnByAngle(double angle);
        void loop();
};

#endif  // _MOTOR_H
