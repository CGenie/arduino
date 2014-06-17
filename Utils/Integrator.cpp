#include "Integrator.h"


// integration starts from 0.0
Integrator::Integrator() {
  reset();
};

Integrator::~Integrator() {
};

void Integrator::step(double val) {
  step(val, val);
};

void Integrator::step(double val, double corrector) {
  unsigned long new_step = millis();

  result = (1.0 - CORRECTOR_WEIGHT)*(result + val*(new_step - last_step)/1000) + CORRECTOR_WEIGHT*corrector;

  last_step = new_step;
};

void Integrator::reset() {
  last_step = millis();
  result = 0.0;
};
