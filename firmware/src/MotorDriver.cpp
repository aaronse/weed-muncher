#include "config.h"
#include "MotorDriver.h"

#ifdef USE_TMC2209
  #define EN_ACTIVE    LOW
  #define EN_INACTIVE  HIGH
  #define TMC_SERIAL   Serial1
  // Sense resistor constant must match your board
  #ifndef R_SENSE
  # define R_SENSE 0.11f
  #endif

  MotorDriver::MotorDriver(uint8_t st, uint8_t di, uint8_t en, uint8_t driverNum)
    : _stepPin(st), _dirPin(di), _enPin(en),
      _driver(&TMC_SERIAL, R_SENSE, driverNum)
  {}

  void MotorDriver::begin() {
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin,  OUTPUT);
    pinMode(_enPin,   OUTPUT);
    TMC_SERIAL.begin(115200);
    _driver.begin();
    _driver.toff(5);
    _driver.rms_current(800);
    //_driver.microsteps(16);

    // Use coarse microstepping for max torque and efficiency.
    // Finer steps (e.g. 16+) reduce torque per pulse and increase CPU load.
    // This isn't a precision CNC — it's a torque-hungry weed puller 😎
    _driver.microsteps(2);  // Options: 2 or 4 give good torque + decent smoothness

    enable(false);
  }

  void MotorDriver::setCurrent(uint16_t mA) {
    _driver.rms_current(mA);
  }

  uint16_t MotorDriver::getDriverVersion() {
    return 2209;
  }

#else
  // A4988 version stub
  #define EN_ACTIVE    LOW
  #define EN_INACTIVE  HIGH

  MotorDriver::MotorDriver(uint8_t st, uint8_t di, uint8_t en)
    : _stepPin(st), _dirPin(di), _enPin(en)
  {}

  void MotorDriver::begin() {
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin,  OUTPUT);
    pinMode(_enPin,   OUTPUT);
    enable(false);
  }

  void MotorDriver::setCurrent(uint16_t) {
    // no-op: fixed by resistor
  }

  uint16_t MotorDriver::getDriverVersion() {
    return 4988;
  }

#endif

void MotorDriver::enable(bool on) {
  digitalWrite(_enPin, on ? EN_ACTIVE : EN_INACTIVE);
}

void MotorDriver::setDirection(bool forward) {
  digitalWrite(_dirPin, forward ? HIGH : LOW);
}

void MotorDriver::stepPulse() {
  digitalWrite(_stepPin, HIGH);
  delayMicroseconds(2);
  digitalWrite(_stepPin, LOW);
}
