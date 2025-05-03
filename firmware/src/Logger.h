// Logger.h
#pragma once
#include <Arduino.h>
#include "config.h"

void initLogger(unsigned long baudRate);
void enableDebug(bool enabled);

void debug(const char* msg);
void debugln(const char* msg);

void debug(int value);
void debugln(int value);

void debug(unsigned int value);
void debugln(unsigned int value);

void debug(long value);
void debugln(long value);

void debug(unsigned long value);
void debugln(unsigned long value);

void debug(float value);
void debugln(float value);

void debug(bool value);
void debugln(bool value);

void debugln(unsigned long value, int format);
void debugln(long value, int format);
void debugln(unsigned int value, int format);
void debugln(int value, int format);


#ifdef ENABLE_DEBUG
  #define DEBUG(msg)    debug(msg)
  #define DEBUGLN(msg)  debugln(msg)
#else
  #define DEBUG(msg)
  #define DEBUGLN(msg)
#endif
