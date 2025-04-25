#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // Initialize serial for debug output
  Serial.begin(9600);
  while (!Serial) { /* wait for Serial port to be ready */ }
  Serial.println("Hello, Weed Muncher!");
  
  // Set up the onboard LED (pin 13 on Nano 33 IoT)
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Blink the LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(250);
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);
  Serial.println("Status: Munch!");
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}