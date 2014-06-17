#include "AccelGyro.h"


Calibrator::Calibrator(char type, char axis, double target) {
  this->type = type;
  this->axis = axis;
  this->target = target;
  this->left = -5000;
  this->right = 5000;
};

Calibrator::~Calibrator() {
};

void Calibrator::next(double val) {
  int16_t middle = (left + right)/2;
  int16_t half_len = (right - left)/2;

  if(error(val) < 0.001) {
    left = middle - half_len;
    right = middle + half_len;
    return;
  };

  if(val - target < 0) {
    left = middle;
  } else {
    right = middle;
  }
};

void Calibrator::setOffset(AccelGyro* ag) {
  ag->setOffset(type, axis, (left + right)/2);
};

void Calibrator::calibrate(AccelGyro* ag, double val) {
  next(val);
  setOffset(ag);
  Serial.print("CALIBRATOR: ");
  Serial.print(type); Serial.print(" ");
  Serial.print(axis); Serial.print(" ");
  Serial.print(val); Serial.print("\t");
  Serial.println((left + right)/2);
};

double Calibrator::error(double val) {
  return abs(val - target);
};

void Calibrator::leftRightReset() {
  left = left - 1000;
  right = right + 1000;

  Serial.print("CALIBRATOR: left: ");
  Serial.print(left);
  Serial.print("\tright: ");
  Serial.println(right);
};


AccelGyro::AccelGyro() {
  accelgyro = new MPU6050();

  axangle = 0.0;
  ayangle = 0.0;
  azangle = 0.0;
  agxint = 0.0;

  gxint = new Integrator();
  gyint = new Integrator();
  gzint = new Integrator();
};

AccelGyro::~AccelGyro() {
  delete accelgyro;
  delete gxint;
  delete gyint;
  delete gzint;
};

void AccelGyro::setup() {
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Serial.println("Wire");
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Serial.println("FastWire");
    Fastwire::setup(400, true);
  #endif

  accelgyro->initialize();

  Serial.println("Testing device connections...");
  Serial.println(accelgyro->testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

  calibrate();

  gxint->reset();
  gyint->reset();
  gzint->reset();
};

void AccelGyro::calibrate() {
  double ax, ay, az, gx, gy, gz;
  double error = 1.0;
  double stop = false;

  Calibrator* cax = new Calibrator('A', 'X', 0.0);
  Calibrator* cay = new Calibrator('A', 'Y', 0.0);
  Calibrator* caz = new Calibrator('A', 'Z', 0.5);
  Calibrator* cgx = new Calibrator('G', 'X', 0.0);
  Calibrator* cgy = new Calibrator('G', 'Y', 0.0);
  Calibrator* cgz = new Calibrator('G', 'Z', 0.0);

  delay(1000);

  while(!stop) {
    for(int i = 0; i < 13; i++) {
      getMotion6AvgNormalized(&ax, &ay, &az, &gx, &gy, &gz);

      error = cax->error(ax) + cay->error(ay) + caz->error(az) + cgx->error(gx) + cgy->error(gy) + cgz->error(gz);
      if(error < 0.01) {
        stop = true;
        break;
      }

      cax->calibrate(this, ax);
      cay->calibrate(this, ay);
      caz->calibrate(this, az);
      cgx->calibrate(this, gx);
      cgy->calibrate(this, gy);
      cgz->calibrate(this, gz);
      Serial.print("error: ");
      Serial.println(error);
      Serial.println();
    };

    cax->leftRightReset();
    cay->leftRightReset();
    caz->leftRightReset();
    cgx->leftRightReset();
    cgy->leftRightReset();
    cgz->leftRightReset();
    Serial.println();
  }

  delete cax;
  delete cay;
  delete caz;
  delete cgx;
  delete cgy;
  delete cgz;
};

double AccelGyro::calibration_error(double ax, double ay, double az, double gx, double gy, double gz) {
  //  return sqrt(
};

void AccelGyro::setOffset(char type, char axis, int16_t offset) {
  switch(type) {
    case 'A':
      setAccOffset(axis, offset);
      break;
    case 'G':
      setGyrOffset(axis, offset);
      break;
  }
};

void AccelGyro::setAccOffset(char axis, int16_t offset) {
  switch(axis) {
    case 'X':
      accelgyro->setXAccelOffset(offset);
      break;
    case 'Y':
      accelgyro->setYAccelOffset(offset);
      break;
    case 'Z':
      accelgyro->setZAccelOffset(offset);
      break;
  }
};

void AccelGyro::setGyrOffset(char axis, int16_t offset) {
  switch(axis) {
    case 'X':
      accelgyro->setXGyroOffset(offset);
      break;
    case 'Y':
      accelgyro->setYGyroOffset(offset);
      break;
    case 'Z':
      accelgyro->setZGyroOffset(offset);
      break;
  }
};

// Return normalized motion values (i.e. in radians)
void AccelGyro::getMotion6Normalized(double* ax, double* ay, double* az, double* gx, double* gy, double* gz) {
  double r = M_PI/180;
  int16_t iax, iay, iaz, igx, igy, igz;

  accelgyro->getMotion6(&iax, &iay, &iaz, &igx, &igy, &igz);

  /*
  Serial.print("getMotion6Normalized: ");
  Serial.print(iax); Serial.print("\t");
  Serial.print(iay); Serial.print("\t");
  Serial.print(iaz); Serial.print("\t");
  Serial.print(igx); Serial.print("\t");
  Serial.print(igy); Serial.print("\t");
  Serial.println(igz);
  */

  *ax = (double)iax/MAXACC;
  *ay = (double)iay/MAXACC;
  *az = (double)iaz/MAXACC;
  *gx = (double)igx/MAXGYR*r*GYRO_DEGREES_PER_SEC;
  *gy = (double)igy/MAXGYR*r*GYRO_DEGREES_PER_SEC;
  *gz = (double)igz/MAXGYR*r*GYRO_DEGREES_PER_SEC;
};

// Probe motion values for some time and return the average
void AccelGyro::getMotion6AvgNormalized(double* ax, double* ay, double* az, double* gx, double* gy, double* gz) {
  double avax, avay, avaz, avgx, avgy, avgz;
  double weight = 0.95;
  unsigned long start = millis();

  getMotion6Normalized(&avax, &avay, &avaz, &avgx, &avgy, &avgz);

  while(millis() - start < 1000) {
    getMotion6Normalized(ax, ay, az, gx, gy, gz);

    avax = weight*avax + (1 - weight)*(*ax);
    avay = weight*avay + (1 - weight)*(*ay);
    avaz = weight*avaz + (1 - weight)*(*az);
    avgx = weight*avgx + (1 - weight)*(*gx);
    avgy = weight*avgy + (1 - weight)*(*gy);
    avgz = weight*avgz + (1 - weight)*(*gz);
  }

  *ax = avax;
  *ay = avay;
  *az = avaz;
  *gx = avgx;
  *gy = avgy;
  *gz = avgz;
};

void AccelGyro::plotMotion6Normalized() {
  double ax, ay, az, gx, gy, gz;

  getMotion6Normalized(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("PLOT: ");
  Serial.print(ax); Serial.print("\t");
  Serial.print(ay); Serial.print("\t");
  Serial.print(az); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.print(gz); Serial.println("\tENDPLOT");
};

// Main angle computation loop
void AccelGyro::compute() {
  int zdirection;
  double ax, ay, az, azn, gx, gy, gz;

  getMotion6Normalized(&ax, &ay, &az, &gx, &gy, &gz);

  zdirection = ay > 0 ? 1 : -1;

  azn = 2*az;
  azn = (azn >= 0.99999) ? 0.99999 : azn;

  azangle = 0.5*azangle + 0.5*zdirection*acos(azn);

  gxint->step(gx, azangle);
  agxint = 0.5*agxint + 0.5*gxint->result;
  //gyint->step(gy);
  //gzint->step(gz);
};

// plot data from compute()
void AccelGyro::plot() {
  double ax, ay, az, gx, gy, gz;

  getMotion6Normalized(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("PLOT: ");
  Serial.print(az);
  Serial.print("\t"); Serial.print(azangle);
  Serial.print("\t"); Serial.print(gxint->result);
  //Serial.print("\t"); Serial.print(gyint->result);
  //Serial.print("\t"); Serial.print(gzint->result);
  Serial.println("\tENDPLOT");
};
