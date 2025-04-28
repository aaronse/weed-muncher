#include "DebouncedButton.h"

DebouncedButton::DebouncedButton(uint8_t p, unsigned long d)
  : pin(p), debounceDelay(d)
{}

void DebouncedButton::begin() {
  pinMode(pin, INPUT_PULLUP);
  lastReading    = digitalRead(pin);
  stableState    = lastReading;
  lastDebounceTime = millis();
}

void DebouncedButton::update() {
  int reading = digitalRead(pin);
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) >= debounceDelay) {
    stableState = reading;
  }
  lastReading = reading;
}

bool DebouncedButton::isPressed() const {
  return stableState == LOW;
}
