#pragma once
#include <Arduino.h>

class DebouncedButton {
public:
  DebouncedButton(uint8_t pin, unsigned long debounceMs = 50);
  void begin();
  void update();
  bool isPressed() const;

private:
  uint8_t pin;
  int     lastReading, stableState;
  unsigned long lastDebounceTime, debounceDelay;
};
