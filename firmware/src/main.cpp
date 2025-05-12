#define USE_TMC2209

#include <Arduino.h>
#include "MotorCommChannel.h"
#include "MotorDriver.h"
#include "DebouncedButton.h"
#include <TMCStepper.h>   // if you use USE_TMC2209
#include <NewPing.h>      // if using sonar
#include <Adafruit_NeoPixel.h>
#include "Logger.h"

// For SERCOM pin binding
#include <wiring_private.h>  

//—[ Configuration ]————————————————————————————————————
#define R_SENSE      0.11f    // Sense resistor on your TMC2209 carrier
#define DRIVER_ADDR  0        // TMC2209 UART address (usually 0 if only one)

#define STEP1_PIN    7 // Was 3 = D3
#define DIR1_PIN     2
#define EN1_PIN      5

#define STEP2_PIN    18
#define DIR2_PIN     17
#define EN2_PIN      20

#define BTN_GRIND    9
#define BTN_TURBO    8
#define ENC_SW       13
#define ENC_CLK      14
#define ENC_DT       15

#define DIAG1_PIN    6
#define DIAG2_PIN    21

#define US_TRIG_PIN  11
#define US_ECHO_PIN  10
#define NEOPIXEL_PIN 12

// 🧠 UART1: TMC Stepper 1
#define TMC1_TX_PIN 4 // WAS 7 = D7
#define TMC1_RX_PIN 3 // WAS 4 = D4

// 🧠 UART2: TMC Stepper 2
#define TMC2_TX_PIN 16  // D16
#define TMC2_RX_PIN 19  // D19

#define US_MAX_CM    200U
#define US_INTERVAL  100U   // ms between pings

#define DEBOUNCE_MS  50U    // button debounce
#define SERIAL_RATE  115200
#define REPORT_MS    1000U // status report interval

#define ENCODER_MIN   0
#define ENCODER_MAX   100
//#define ENCODER_STEP  1 // 0-100% in 720deg rotation, fine tuning - thumb/finger fatigue 
#define ENCODER_STEP  4   // 0-100% in 180deg rotation, coarse tuning - responsive 

volatile int encoderPos = 50; // Start at middle speed
int lastEncoded = 0;
bool lastEncA = HIGH;

#define NUM_PIXELS 1
Adafruit_NeoPixel statusPixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// map analog→steps-per-second
static const uint32_t MAX_SPEED_SPS   = 2000;
static const uint32_t TURBO_SPEED_SPS = 4000;


//—[ Globals & Objects ]————————————————————————————
MotorCommChannel comm1(
  "Stepper1",
  &sercom2,
  TMC1_TX_PIN,
  UART_TX_PAD_0,    // TX = PAD0
  TMC1_RX_PIN,
  SERCOM_RX_PAD_1   // RX = PAD1
);

MotorCommChannel comm2(
  "Stepper2",
  &sercom3,
  TMC2_TX_PIN, 
  UART_TX_PAD_0,      // D16 (PA22) is PAD0
  TMC2_RX_PIN,
  SERCOM_RX_PAD_1     // D19 (PA23) is PAD1
);

MotorDriver motor1(STEP1_PIN, DIR1_PIN, EN1_PIN, &comm1, 0);
MotorDriver motor2(STEP2_PIN, DIR2_PIN, EN2_PIN, &comm2, 0);

DebouncedButton btnGrind(BTN_GRIND);
DebouncedButton btnTurbo(BTN_TURBO);
DebouncedButton btnEncoderSW(ENC_SW);
NewPing sonar(US_TRIG_PIN, US_ECHO_PIN, US_MAX_CM);

enum State { ST_IDLE=0, ST_GRIND, ST_REVERSE, ST_TURBO };
State   currentState = ST_IDLE;
uint32_t lastStepUs   = 0;
uint32_t lastReportMs = 0;
uint32_t lastPingMs   = 0;
uint16_t usDistCm     = 0;


//—[ Forward Declarations ]—————————————————————————————————————————
void handleEncoder();
void updateUltrasound();
void stepMotors();
const char* stateName(State s);


//—[ Setup ]—————————————————————————————————————————
void setup() {  
  initLogger(115200);  // Pass baud rate explicitly
  debugln("🛠️  Weed Muncher Starting...");
  debug("getDriverVersion: ");
  debugln(motor1.getDriverVersion());
  

  // Set up the onboard LED (Normally pin 13 on Nano 33 IoT)
  pinMode(LED_BUILTIN, OUTPUT);

  btnGrind.begin();
  btnTurbo.begin();
  btnEncoderSW.begin();
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(DIAG1_PIN, INPUT);
  pinMode(DIAG2_PIN, INPUT);

  comm1.begin();
  comm2.begin();
  motor1.begin();
  motor2.begin();
  motor1.maximizeTorque();
  motor2.maximizeTorque();

  delay(100); // Wait for motors to initialize

  debug("Motor1 UART test: "); 
  debugln(motor1.driver()->test_connection());
  //debug("Motor2 UART test: "); 
  //debugln(motor2.driver()->test_connection());

  motor1.debugDriver("Startup Motor1");
  motor2.debugDriver("Startup Motor2");


  // NeoPixel init
  statusPixel.begin();
  statusPixel.setBrightness(32);
  statusPixel.show();  // turn off
}


//—[ Main Loop ]———————————————————————————————————
void loop() {
  handleEncoder();

  // 1) update inputs
  btnGrind.update();
  btnEncoderSW.update();
  btnTurbo.update();
  updateUltrasound();

  // 2) determine new state
  State newState = ST_IDLE;
  if (btnGrind.isPressed() && btnTurbo.isPressed()) newState = ST_REVERSE;
  else if (btnTurbo.isPressed())                    newState = ST_TURBO;
  else if (btnGrind.isPressed())                    newState = ST_GRIND;
  
  // 3) on state change, (re)configure motors
  if (newState != currentState) {
    currentState = newState;
    if (currentState == ST_IDLE) {
      motor1.enable(false);
      motor2.enable(false);
      statusPixel.setPixelColor(0, statusPixel.Color(0, 0, 0));  // off
    } else {
      bool forward = (currentState != ST_REVERSE);
      motor1.setDirection(forward);
      motor2.setDirection(!forward);
      motor1.enable(true);
      motor2.enable(true);

      // Wait for motors to stabilize after enabling
      delay(100); 

      // Optional: scale current from 400mA to 1200mA based on encoderPos
      // uint16_t targetCurrent = map(encoderPos, ENCODER_MIN, ENCODER_MAX, 400, 1400); // Weak current at low speed was causing stalling
      uint16_t targetCurrent = (currentState == ST_TURBO) ? 1400 : 1000;
      motor1.setCurrent(targetCurrent);
      motor2.setCurrent(targetCurrent);
      
      motor1.debugDriver("Mode Change Motor1");
      motor2.debugDriver("Mode Change Motor2");
      

      // Set color by mode
      uint32_t color = 0;
      if (currentState == ST_GRIND)   color = statusPixel.Color(0, 255, 0);  // Green
      if (currentState == ST_TURBO)   color = statusPixel.Color(255, 255, 0);  // Yellow
      if (currentState == ST_REVERSE) color = statusPixel.Color(0, 0, 255);  // Blue
    
      statusPixel.setPixelColor(0, color);
    }

    statusPixel.show();
  }

  // 4) step if needed
  stepMotors();

  // 5) periodic status report
  if (millis() - lastReportMs >= REPORT_MS) {
    lastReportMs = millis();
    debug("State="); debug(stateName(currentState));
    debug("  EncoderPos="); debug(encoderPos);
    debug("  Dist="); debug(usDistCm); debug("cm");
    debug("  Grind=");   debug(btnGrind.isPressed());
    debug("  Turbo=");   debug(btnTurbo.isPressed());
    debug("  SW=");      debugln(btnEncoderSW.isPressed());
  }
  
  // Let the MCU breathe; saves power and reduces noise
  if (currentState == ST_IDLE) {
    // TODO:P1 Power Saving, dynamically increase delay to 50ms when no recent interaction.  Goal is to sleep longer in idle to reduce power
    delay(5);  
  }
}

// TODO:P1 Optional: Dynamic Sensitivity (e.g., Fast Spin = Bigger Steps)
void handleEncoder() {
  bool encA = digitalRead(ENC_CLK);
  bool encB = digitalRead(ENC_DT);

  // Only act on changes to A (rising/falling)
  if (encA != lastEncA) {
    lastEncA = encA;
    if (encA == encB) encoderPos += ENCODER_STEP;  // Clockwise
    else              encoderPos -= ENCODER_STEP;  // Counter-clockwise
    encoderPos = constrain(encoderPos, ENCODER_MIN, ENCODER_MAX);
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
  uint32_t baseMax = (currentState == ST_TURBO) ? TURBO_SPEED_SPS : MAX_SPEED_SPS;
  speedSps = map(encoderPos, ENCODER_MIN, ENCODER_MAX, 0, baseMax);
  
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
