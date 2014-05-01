// Some data taken from
// http://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/step4/One-Motor/

#include <Arduino.h>

#include "BoolPin.h"

#ifndef _MOTOR_H
#define _MOTOR_H

class Motor {
    private:
        char variant;  // 'A' or 'B'

        BoolPin* directionPin;
        BoolPin* brakePin;
        int analogSpeedPin;

    public:
        Motor(char variant);
        ~Motor();
        
        void setup();
        
        void setSpeed(int speed);

        // generic function for setting motor speed & direction
        // speed > 0 => forward, speed < 0 => backward
        void go(int speed);
};

#endif  // _MOTOR_H
