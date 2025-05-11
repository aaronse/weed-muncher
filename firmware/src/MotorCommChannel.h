#pragma once
#include "config.h"
#include <Arduino.h>
#include <TMCStepper.h>
#include <wiring_private.h>
#include "sam.h"          // Gives you SERCOM*

/**
 * ⚠️ SERCOM UART Routing (SAMD21/Nano 33 IoT)
 * -------------------------------------------
 * The Arduino Nano 33 IoT uses the SAMD21 microcontroller, which has 6 flexible SERCOM blocks 
 * (SERCOM0–SERCOM5).  Each SERCOM can be assigned to I2C, SPI, or UART — but some are already 
 * used by the core framework:
 * 
 *   - SERCOM0: Reserved for USB Serial (Serial)
 *   - SERCOM1: Used by Wire (I2C) — do NOT reuse
 *   - SERCOM2: Often used for SPI (MOSI/SCK/MISO) — reuse with caution
 *   - ✅ SERCOM3, SERCOM4, SERCOM5: Generally free for custom UARTs (safe for stepper UART)
 *
 * If two `Uart` instances are mapped to the same SERCOM (e.g. both use SERCOM1), the MCU may 
 * freeze or behave unpredictably.  Always use different SERCOMs for each UART instance.
 *
 * ESP32 NOTE:
 * - ESP32 has fixed UART peripherals (UART0–UART2), but allows TX/RX pins to be assigned freely 
 *    with `SerialX.begin(baud, config, rx, tx)`
 * - No SERCOM-like conflicts — just avoid overlapping RX/TX pins
 *
 * Summary:
 * - ✅ Use SERCOM3+ for TMC2209 stepper UARTs
 * - ✅ Use unique SERCOM per driver
 * - ❌ Do not use SERCOM0, SERCOM1 unless you're overriding system I/O intentionally
 */

/**
 * MotorCommChannel
 * -----------------
 * Encapsulates the communication interface between the MCU and one or more TMC2209 stepper drivers.
 * Supports both:
 *   - One TX + one RX per driver (current config)
 *   - One shared TX + many RX for multi-driver UART (future)
 * 
 * Responsibilities:
 *   - Configure and initialize SERCOM UART (SAMD) or ESP32 UART
 *   - Encapsulate TX/RX pin mappings
 *   - Provide a Stream* for use with TMCStepper
 * 
 * Designed to be extended for future protocols (e.g., SPI, I2C) or diagnostics (e.g., DIAG pin)
 */ 
class MotorCommChannel {
public:
  MotorCommChannel(
    const char* name,              // Friendly label for debug output
    SERCOM* sercom,                // SERCOM peripheral instance (SERCOM3, etc.)
    uint8_t txPin,                 // GPIO for UART TX
    uint8_t rxPin,                 // GPIO for UART RX
    SercomRXPad rxPad,             // Which SERCOM PAD the RX pin maps to (PAD0–3)
    SercomUartTXPad txPad          // Which SERCOM TX PAD (e.g., UART_TX_PAD_0)
    );

  void begin();                    // Maps pins to UART and starts serial
  Stream* getStream();             // Used by MotorDriver/TMCStepper
  const char* getName() const;    // For debug purposes

private:
  const char* _name;
  uint8_t _txPin, _rxPin;
  SERCOM* _sercom;
  SercomRXPad _rxPad;
  SercomUartTXPad _txPad;
  Uart _uart;
};
