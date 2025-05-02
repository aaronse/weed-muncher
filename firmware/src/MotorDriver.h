#pragma once
#include "config.h"
#include <Arduino.h>

#ifdef USE_TMC2209
  #include <TMCStepper.h>
#endif

class MotorDriver {
public:
  MotorDriver(uint8_t stepPin, uint8_t dirPin, uint8_t enPin, uint8_t driverNum=0);
  void begin();
  void setCurrent(uint16_t mA);
  void enable(bool on);
  void setDirection(bool forward);
  void stepPulse();
  uint16_t getDriverVersion();
private:
  uint8_t _stepPin, _dirPin, _enPin;
#ifdef USE_TMC2209
  TMC2209Stepper _driver;
#endif
};
