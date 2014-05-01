// taken from
// http://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/step4/One-Motor/

#include <stdlib.h>  // abs

#include "Motor.h"


Motor::Motor(char variant) {
    // channel A data
    this->variant = variant;

    if(variant == 'A') {
        this->directionPin = new BoolPin(12);
        this->brakePin = new BoolPin(9);
        this->analogSpeedPin = 3;
    } else {
        this->directionPin = new BoolPin(12);
        this->brakePin = new BoolPin(9);
        this->analogSpeedPin = 3;
    }
};

Motor::~Motor() {
    delete this->directionPin;
    delete this->brakePin;
};

void Motor::setup() {
    this->directionPin->setup();
    this->brakePin->setup();
};

void Motor::setSpeed(int speed) {
    analogWrite(this->analogSpeedPin, speed);
}

// generic function for setting motor speed & direction
// speed > 0 => forward, speed < 0 => backward
void Motor::go(int speed) {
    bool speedDirection = (speed > 0);

    if(speed == 0) {
        this->brakePin->on();
        this->setSpeed(0);
        return;
    }

    if(this->directionPin->state != speedDirection) {
      this->brakePin->on();
      delay(5);
    }

    this->directionPin->setState(speedDirection);
    this->brakePin->off();

    this->setSpeed(abs(speed));
}
