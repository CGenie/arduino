// BoolPin -- class representing a single output pin
// Supports inverted pins:
// inverted == false: on -> HIGH, off -> LOW
// inverted == true:  on -> LOW, off -> HIGH

#include <Arduino.h>

#ifndef _BOOL_PIN_H
#define _BOOL_PIN_H


class BoolPin {
    public:
        int pin;  // pin number
        bool inverted;  // whether the pin is inverted
        bool state;  // current pin state (false -> off, true -> on)

        BoolPin(int pin, bool inverted=false, bool state=false);

        ~BoolPin();

        void setup(bool initialState=false);

        void setState(bool state);
        void on();
        void off();
};

#endif // _BOOL_PIN_H
