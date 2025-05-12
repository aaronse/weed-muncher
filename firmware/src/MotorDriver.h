#pragma once
#include "config.h"
#include <Arduino.h>
#include "MotorCommChannel.h"

#ifdef USE_TMC2209
  #include <TMCStepper.h>
#endif

/**
 * MotorDriver
 * -------------
 * Controls a stepper motor using step/dir/enable pins and a smart driver (e.g. TMC2209).
 * 
 * Communicates via a MotorCommChannel which encapsulates transport/config (UART/SPI/etc).
 * Responsible for motion control, torque settings, and high-level behavior.
 */
class MotorDriver {
public:
  MotorDriver(uint8_t stepPin, uint8_t dirPin, uint8_t enPin, MotorCommChannel* comm, uint8_t driverAddr = 0);
  void begin();
  void setCurrent(uint16_t mA);
  void enable(bool on);
  void setDirection(bool forward);
  void stepPulse();
  uint16_t getDriverVersion();
  void maximizeTorque();
  void debugDriver(const char* label = nullptr);
  TMC2209Stepper* driver() { return &_driver; }
  bool isConnected() const { return _isConnected; }
  bool isConnected(bool autoReconnect);
private:
  uint8_t _stepPin, _dirPin, _enPin;
  MotorCommChannel* _comm;
#ifdef USE_TMC2209
  TMC2209Stepper _driver;
#endif
  bool _isConnected = false; // Tracks connection status
  unsigned long _lastConnectionCheck = 0;
};
