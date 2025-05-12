#include "MotorCommChannel.h"

MotorCommChannel::MotorCommChannel(
  const char* name,
  SERCOM* sercom,
  uint8_t txPin,
  SercomUartTXPad txPad,
  uint8_t rxPin,
  SercomRXPad rxPad
)
  : _name(name),
    _txPin(txPin),
    _rxPin(rxPin),
    _sercom(sercom),
    _rxPad(rxPad),
    _txPad(txPad),
    _uart(sercom, rxPin, txPin, rxPad, txPad)
{}


void MotorCommChannel::begin() {
  pinPeripheral(_rxPin, PIO_SERCOM);
  pinPeripheral(_txPin, PIO_SERCOM);
  _uart.begin(115200); // Try lower if suspect poor wiring e.g. 57600
}

Stream* MotorCommChannel::getStream() {
  return &_uart;
}

const char* MotorCommChannel::getName() const {
  return _name;
}
