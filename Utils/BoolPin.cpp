#include "BoolPin.h"


BoolPin::BoolPin(int pin, char type, bool inverted, bool state) {
    this->type = type;
    this->pin = pin;
    this->inverted = inverted;
    this->state = state;
}

BoolPin::~BoolPin() {};

void BoolPin::setup(bool initialState) {
    if(type == 'I') {
      pinMode(this->pin, INPUT);
      readState();
    } else {
      pinMode(this->pin, OUTPUT);

      if(initialState) {
        this->on();
      } else {
        this->off();
      }
    }
}

bool BoolPin::readState() {
  state = digitalRead(this->pin);

  return (state && !this->inverted) ? state : !state;
};

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
