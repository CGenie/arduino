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

    this->encoderPin = new BoolPin(21, 'I');

    this->currentSpeed = 0;
    this->encoderCount = 0;
    this->shaftRatio = 29;  // if motor has a shaft, this is its ratio
    this->targetAngle = 0.0;
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
    delete this->encoderPin;
};

void Motor::setup() {
    this->directionPin->setup();
    this->brakePin->setup();
    this->encoderPin->setup();
    //    attachInterrupt(5, &Motor::computeEncoder, RISING);  // int 5 on pin 18
};

void Motor::setSpeed(int speed) {
    analogWrite(this->analogSpeedPin, speed);
}

double Motor::getEncoderAngle() {
  // we're using only one Hall input so angle is 1/16 of turn
  double ret = 2.0*M_PI*this->encoderCount/(16.0*this->shaftRatio);
  return ret;
  if(ret > 0) {
    while(ret >= 2*M_PI) {
      ret -= 2*M_PI;
    }
  } else {
    while(ret <= 0) {
      ret += 2*M_PI;
    }
  };

  return ret;
};

// generic function for setting motor speed & direction
// speed > 0 => forward, speed < 0 => backward
void Motor::go(int speed) {
    bool speedDirection = (speed > 0);
    this->currentSpeed = speed;
    speedDirection = this->reversed ? !speedDirection : speedDirection;

    if(speed == 0) {
        this->brakePin->on();
        this->setSpeed(0);
        return;
    }

    if(this->directionPin->state != speedDirection) {
      this->brakePin->on();
      // delay(5);
    }

    this->directionPin->setState(speedDirection);
    this->brakePin->off();

    this->setSpeed(abs(speed));
}

void Motor::computeEncoder() {
  if(this->currentSpeed == 0) {
    return;
  }

  this->currentSpeed > 0 ? this->encoderCount++ : this->encoderCount--;
};

void Motor::turnByAngle(double angle) {
  this->targetAngle = angle;
};

// main looping function
void Motor::loop() {
  double angle = this->getEncoderAngle();
  int direction;

  if(abs(angle - targetAngle) > 0.001) {
    // turn
    direction = angle > targetAngle ? -1 : 1;
    this->go(direction*100);
  } else {
    this->go(0);
  }
};
