#define USE_TMC2209

#include <Arduino.h>
#include "MotorDriver.h"
#include "DebouncedButton.h"
#include <TMCStepper.h>   // if you use USE_TMC2209
#include <NewPing.h>      // if using sonar

//—[ Configuration ]————————————————————————————————————
#define R_SENSE      0.11f    // Sense resistor on your TMC2209 carrier
#define DRIVER_ADDR  0        // TMC2209 UART address (usually 0 if only one)

#define STEP1_PIN    2
#define DIR1_PIN     3
#define EN1_PIN      4
#define STEP2_PIN    5
#define DIR2_PIN     6
#define EN2_PIN      7

#define BTN_GRIND    8
#define BTN_REVERSE  9
#define BTN_TURBO    10
#define POT_SPEED    A0

#define US_TRIG_PIN  11
#define US_ECHO_PIN  12
#define US_MAX_CM    200U
#define US_INTERVAL  100U   // ms between pings

#define DEBOUNCE_MS  50U    // button debounce
#define SERIAL_RATE  115200
#define REPORT_MS    1000U // status report interval

// map analog→steps-per-second
static const uint32_t MAX_SPEED_SPS   = 2000;
static const uint32_t TURBO_SPEED_SPS = 4000;


//—[ Globals & Objects ]————————————————————————————
MotorDriver motor1(STEP1_PIN, DIR1_PIN, EN1_PIN);
MotorDriver motor2(STEP2_PIN, DIR2_PIN, EN2_PIN);
DebouncedButton btnGrind(BTN_GRIND), btnReverse(BTN_REVERSE), btnTurbo(BTN_TURBO);
NewPing sonar(US_TRIG_PIN, US_ECHO_PIN, US_MAX_CM);

enum State { ST_IDLE=0, ST_GRIND, ST_REVERSE, ST_TURBO };
State   currentState = ST_IDLE;
uint32_t lastStepUs   = 0;
uint32_t lastReportMs = 0;
uint32_t lastPingMs   = 0;
uint16_t usDistCm     = 0;


//—[ Forward Declarations ]—————————————————————————————————————————
void updateUltrasound();
void stepMotors();
const char* stateName(State s);


//—[ Setup ]—————————————————————————————————————————
void setup() {  
  Serial.begin(SERIAL_RATE);
  while (!Serial) {}
  Serial.println("🛠️  Weed Muncher Starting...");

  // Set up the onboard LED (pin 13 on Nano 33 IoT)
  pinMode(LED_BUILTIN, OUTPUT);

  motor1.begin();
  motor2.begin();
  btnGrind.begin();
  btnReverse.begin();
  btnTurbo.begin();
}


//—[ Main Loop ]———————————————————————————————————
void loop() {
  // 1) update inputs
  btnGrind.update();
  btnReverse.update();
  btnTurbo.update();
  updateUltrasound();

  // 2) determine new state
  State newState = ST_IDLE;
  if      (btnTurbo.isPressed())   newState = ST_TURBO;
  else if (btnGrind.isPressed())   newState = ST_GRIND;
  else if (btnReverse.isPressed()) newState = ST_REVERSE;

  // 3) on state change, (re)configure motors
  if (newState != currentState) {
    currentState = newState;
    if (currentState == ST_IDLE) {
      motor1.enable(false);
      motor2.enable(false);
    } else {
      bool forward = (currentState != ST_REVERSE);
      motor1.setDirection(forward);
      motor2.setDirection(!forward);
      motor1.enable(true);
      motor2.enable(true);
    }
  }

  // 4) step if needed
  stepMotors();

  // 5) periodic status report
  if (millis() - lastReportMs >= REPORT_MS) {
    lastReportMs = millis();
    Serial.print("State="); Serial.print(stateName(currentState));
    Serial.print("  SpeedPot="); Serial.print(map(analogRead(POT_SPEED),0,4095,0,100)); Serial.print("%");
    Serial.print("  Dist="); Serial.print(usDistCm); Serial.print("cm");
    Serial.print("  Grind=");   Serial.print(btnGrind.isPressed());
    Serial.print("  Rev=");     Serial.print(btnReverse.isPressed());
    Serial.print("  Turbo=");   Serial.println(btnTurbo.isPressed());
  }
}

//—[ State → Name ]——————————————————————————————
const char* stateName(State s) {
  switch(s) {
    case ST_GRIND:   return "GRIND";
    case ST_REVERSE: return "REVERSE";
    case ST_TURBO:   return "TURBO";
    default:         return "IDLE";
  }
}


//—[ Stepper Tick ]———————————————————————————————————
void stepMotors() {
  if (currentState == ST_IDLE) return;

  // 1) decide target speed in steps/sec
  uint32_t speedSps;
  if (currentState == ST_TURBO) {
    speedSps = TURBO_SPEED_SPS;
  } else {
    // forward or reverse both use the pot
    speedSps = map(analogRead(POT_SPEED), 0, 4095, 0, MAX_SPEED_SPS);
  }

  // 2) skip stepping if zero
  if (speedSps == 0) return;

  // 3) compute interval (no digit separators)
  uint32_t intervalUs = 1000000UL / speedSps;

  // 4) non-blocking timing
  static uint32_t lastStepUs = 0;
  uint32_t nowUs = micros();
  if (nowUs - lastStepUs < intervalUs) return;
  lastStepUs = nowUs;

  // 5) fire the pulses
  motor1.stepPulse();
  motor2.stepPulse();
}


//—[ HC-SR04 Ping ]———————————————————————————————————
void updateUltrasound() {
  uint32_t now = millis();
  if (now - lastPingMs >= US_INTERVAL) {
    lastPingMs   = now;
    usDistCm     = sonar.ping_cm();
  }
}
