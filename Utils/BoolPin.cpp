#include "BoolPin.h"


BoolPin::BoolPin(int pin, bool inverted, bool state) {
    this->pin = pin;
    this->inverted = inverted;
    this->state = state;
}

BoolPin::~BoolPin() {};

void BoolPin::setup(bool initialState) {
    pinMode(this->pin, OUTPUT);

    if(initialState) {
        this->on();
    } else {
        this->off();
    }
}

void BoolPin::setState(bool state) {
    this->state = state;
    digitalWrite(
        this->pin,
        (state && !this->inverted) ? HIGH : LOW);
}

void BoolPin::on() {
    this->setState(true);
}

void BoolPin::off() {
    this->setState(false);
}
