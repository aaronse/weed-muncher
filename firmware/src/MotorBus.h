// MotorBus.h
#pragma once
#include <Arduino.h>
#include <Stream.h>
#include <wiring_private.h>  // For SAMD pinPeripheral()

class MotorBus {
public:
  MotorBus(const char* name, Uart& uart, uint8_t txPin, uint8_t rxPin, uint8_t diagPin = 0);

  void begin();
  Stream* getStream() const;
  uint8_t getDiagPin() const;
  const char* getName() const;

private:
  const char* _name;
  Uart& _uart;
  uint8_t _txPin;
  uint8_t _rxPin;
  uint8_t _diagPin;
};

// MotorBus.cpp
#include "MotorBus.h"

MotorBus::MotorBus(const char* name, Uart& uart, uint8_t txPin, uint8_t rxPin, uint8_t diagPin)
  : _name(name), _uart(uart), _txPin(txPin), _rxPin(rxPin), _diagPin(diagPin) {}

void MotorBus::begin() {
  pinPeripheral(_rxPin, PIO_SERCOM);
  pinPeripheral(_txPin, PIO_SERCOM);
  _uart.begin(115200);
}

Stream* MotorBus::getStream() const {
  return &_uart;
}

uint8_t MotorBus::getDiagPin() const {
  return _diagPin;
}

const char* MotorBus::getName() const {
  return _name;
} 
