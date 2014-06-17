// Time-integration helper

#include <Arduino.h>

#ifndef _INTEGRATOR_H
#define _INTEGRATOR_H

class Integrator {
    private:
        static const double CORRECTOR_WEIGHT = 0.02;
        unsigned long last_step;

    public:
        double result;

        Integrator();
        ~Integrator();

        void reset();
        void step(double val);
        void step(double val, double corrector);
};

#endif  // _INTEGRATOR_H
