// taken from
// http://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/step4/One-Motor/

#include <stdlib.h>  // abs

#include "Motor.h"


void Motor::init(char variant, bool reversed) {
    // channel A data
    this->variant = variant;

    this->reversed = reversed;

    if(variant == 'A') {
        this->directionPin = new BoolPin(12);
        this->brakePin = new BoolPin(9);
        this->analogSpeedPin = 3;
        // current sensing -> pin Analog 0
    } else {
        this->directionPin = new BoolPin(13);
        this->brakePin = new BoolPin(8);
        this->analogSpeedPin = 11;
        // current sensing -> pin Analog 1
    }
};

Motor::Motor(char variant) {
    init(variant, false);
};

Motor::Motor(char variant, bool reversed) {
    init(variant, reversed);
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
    speedDirection = this->reversed ? !speedDirection : speedDirection;

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
