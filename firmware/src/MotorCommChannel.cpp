#include "MotorCommChannel.h"

MotorCommChannel::MotorCommChannel(
  const char* name,
  SERCOM* sercom,
  uint8_t txPin,
  uint8_t rxPin,
  SercomRXPad rxPad,
  SercomUartTXPad txPad
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
  _uart.begin(115200);
}

Stream* MotorCommChannel::getStream() {
  return &_uart;
}

const char* MotorCommChannel::getName() const {
  return _name;
}
