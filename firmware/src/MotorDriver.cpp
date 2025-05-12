#include "config.h"
#include "MotorDriver.h"
#include "Logger.h"

#ifdef USE_TMC2209
  #define EN_ACTIVE    LOW
  #define EN_INACTIVE  HIGH
  //#define TMC_SERIAL   Serial1
  // Sense resistor constant must match your board
  #ifndef R_SENSE
  # define R_SENSE 0.11f
  #endif

  // TODO:P0 Should reset and/or periodically re-evaluate _isConnected

  MotorDriver::MotorDriver(uint8_t st, uint8_t di, uint8_t en, MotorCommChannel* comm, uint8_t driverAddr)
  : _stepPin(st), _dirPin(di), _enPin(en),
    _comm(comm),
    _driver(comm->getStream(), R_SENSE, driverAddr)
  {}

  void MotorDriver::begin() {
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin,  OUTPUT);
    pinMode(_enPin,   OUTPUT);
    
    // Allow time for driver to power up
    enable(true);
    delay(100); 

    _driver.begin();
    _driver.toff(5);
    _driver.rms_current(800);
    //_driver.microsteps(16);

    // Use coarse microstepping for max torque and efficiency.
    // Finer steps (e.g. 16+) reduce torque per pulse and increase CPU load.
    // This isn't a precision CNC — it's a torque-hungry weed puller 😎
    _driver.microsteps(2);  // Options: 2 or 4 give good torque + decent smoothness

    // 🔍 UART test
    int result = _driver.test_connection();
    _isConnected = (result == 0);
    debug("["); debug(_comm->getName()); debug("] ");
    if (_isConnected) {
      debugln("✅ UART connection OK");
    } else {
      debug("❌ UART connection FAIL (code: "); debug(result); debugln(")");
    }
    
    enable(false);
  }

  void MotorDriver::setCurrent(uint16_t mA) {
    if (isConnected(true)) {
      _driver.rms_current(mA);
    }
  }

  uint16_t MotorDriver::getDriverVersion() {
    return 2209;
  }

  bool MotorDriver::isConnected(bool autoReconnect) {
    unsigned long now = millis();
  
    if (autoReconnect && (now - _lastConnectionCheck >= 500)) {
      _lastConnectionCheck = now;
  
      int result = _driver.test_connection();
      _isConnected = (result == 0);
  
      debug("["); 
      debug(_comm->getName()); 
      debug("] Re-check UART connection: ");
      if (_isConnected) {
        debugln("✅ OK");
      } else {
        debug("❌ FAIL (code: "); 
        debug(result); 
        debugln(")");
      }
    }
  
    return _isConnected;
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


// void MotorDriver::maximizeTorque() {
//   _driver.begin();
//   _driver.toff(5);
//   _driver.rms_current(1400);          // 2.0A peak ≈ 1.41A RMS
//   _driver.microsteps(2);              // Fewer microsteps = more torque
//   _driver.en_spreadCycle(true);       // Use spreadCycle instead of stealthChop
// }

void MotorDriver::maximizeTorque() {
  _driver.begin();
  _driver.toff(5);
  _driver.rms_current(1400);              // ≈ 2.0A peak
  _driver.microsteps(2);
  _driver.en_spreadCycle(true);           // Ensure spreadCycle for torque
  _driver.pwm_autoscale(false);           // Optional: disable stealthChop tuning
  _driver.pwm_autograd(false);            // Optional: disable auto PWM
  _driver.I_scale_analog(false);          // Use internal Vref instead of analog pin
}


void MotorDriver::debugDriver(const char* label) {
  if (!_isConnected) {
    debug("["); 
    debug(_comm->getName()); 
    debug(",");
    debug(label);
    debugln("] ⚠️ UART unavailable — skipping Driver debug logic.");
    return;
  }

  if (label) {
    debug("[Driver Debug] "); debug(label); debugln(":");
  }

  uint32_t ihold_irun = _driver.IHOLD_IRUN();
  uint8_t ihold = ihold_irun & 0x1F;
  uint8_t irun  = (ihold_irun >> 8) & 0x1F;
  uint8_t delay = (ihold_irun >> 16) & 0x0F;

  debug("  IHOLD: "); debug(ihold);
  debug("  IRUN: ");  debug(irun);
  debug("  IHOLDDELAY: "); debugln(delay);
  debug("  GCONF: "); debugln(_driver.GCONF(), HEX);
  debug("  IOIN:  "); debugln(_driver.IOIN(), HEX);

  float irun_ratio = irun / 31.0;
  float approx_rms = irun_ratio * 2000.0; // if using 2.0A motors
  debug("Estimated RMS: "); debugln(approx_rms);  
}
