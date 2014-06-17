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

  if(abs(val - target) < 0.0001) {
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


AccelGyro::AccelGyro() {
  accelgyro = new MPU6050();
};

AccelGyro::~AccelGyro() {
  delete accelgyro;
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
};

void AccelGyro::calibrate() {
  double ax, ay, az, gx, gy, gz;

  Calibrator* cax = new Calibrator('A', 'X', 0.0);
  Calibrator* cay = new Calibrator('A', 'Y', 0.0);
  Calibrator* caz = new Calibrator('A', 'Z', 0.5);
  Calibrator* cgx = new Calibrator('G', 'X', 0.0);
  Calibrator* cgy = new Calibrator('G', 'Y', 0.0);
  Calibrator* cgz = new Calibrator('G', 'Z', 0.0);

  delay(1000);

  for(int i = 0; i < 15; i++) {
    getMotion6AvgNormalized(&ax, &ay, &az, &gx, &gy, &gz);

    cax->calibrate(this, ax);
    cay->calibrate(this, ay);
    caz->calibrate(this, az);
    cgx->calibrate(this, gx);
    cgy->calibrate(this, gy);
    cgz->calibrate(this, gz);
    Serial.println();
  };

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

void AccelGyro::getMotion6Normalized(double* ax, double* ay, double* az, double* gx, double* gy, double* gz) {
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
  *gx = (double)igx/MAXGYR;
  *gy = (double)igy/MAXGYR;
  *gz = (double)igz/MAXGYR;
};

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
