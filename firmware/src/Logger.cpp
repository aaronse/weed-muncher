// Logger.cpp
#include "Logger.h"

#ifdef ENABLE_DEBUG
static bool debugEnabled = false;
#endif

void initLogger(unsigned long baudRate) {
#ifdef ENABLE_DEBUG
  Serial.begin(baudRate);
  delay(3000); // Briefly allow time for USB to enumerate
  
  //debugEnabled = Serial; // Only true if host is connected
  // Always true, since time for Serial to establish is unpredictable, and should
  // not block the main loop.  Someone investigating startup issues will need to
  // add a delay to the main loop to allow time for USB connection...
  // e.g. delay(5000);  // 5 seconds for USB to establish
  debugEnabled = true; 
#endif
}

void enableDebug(bool enabled) {
#ifdef ENABLE_DEBUG
  debugEnabled = enabled;
#endif
}

// --- debug() overloads ---

void debug(const char* msg) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.print(msg);
#endif
}

void debug(int value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.print(value);
#endif
}

void debug(unsigned int value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.print(value);
#endif
}

void debug(long value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.print(value);
#endif
}

void debug(unsigned long value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.print(value);
#endif
}

void debug(float value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.print(value);
#endif
}

void debug(bool value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.print(value);
#endif
}

// --- debugln() overloads ---

void debugln(const char* msg) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.println(msg);
#endif
}

void debugln(int value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.println(value);
#endif
}

void debugln(unsigned int value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.println(value);
#endif
}

void debugln(long value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.println(value);
#endif
}

void debugln(unsigned long value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.println(value);
#endif
}

void debugln(float value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.println(value);
#endif
}

void debugln(bool value) {
#ifdef ENABLE_DEBUG
  if (debugEnabled) Serial.println(value);
#endif
}

void debugln(unsigned long value, int format) {
  #ifdef ENABLE_DEBUG
    if (debugEnabled) Serial.println(value, format);
  #endif
  }
  
  void debugln(long value, int format) {
  #ifdef ENABLE_DEBUG
    if (debugEnabled) Serial.println(value, format);
  #endif
  }
  
  void debugln(unsigned int value, int format) {
  #ifdef ENABLE_DEBUG
    if (debugEnabled) Serial.println(value, format);
  #endif
  }
  
  void debugln(int value, int format) {
  #ifdef ENABLE_DEBUG
    if (debugEnabled) Serial.println(value, format);
  #endif
  }
  