#pragma once

#include <Arduino.h>

enum LogLevel {
  ERROR,
  WARN,
  INFO,
  DEBUG,
};

// Minimum severity that gets printed. Messages more verbose than this
// (i.e. with a numerically larger level) are dropped.
extern LogLevel currentLogLevel;

// printf-style logger. msg is formatted with params just like printf, and a
// trailing '\n' is always appended.
void log(LogLevel level, const char* msg, ...);
