#pragma once

#include <Arduino.h>

enum LogLevel {
  ERROR,
  WARN,
  INFO,
  DEBUG,
};

// Compile-time default for the minimum severity printed. Override at build time
// with a flag, e.g. -DLOG_LEVEL=INFO (value is one of the LogLevel names).
#ifndef LOG_LEVEL
#define LOG_LEVEL INFO
#endif

// Minimum severity that gets printed. Messages more verbose than this
// (i.e. with a numerically larger level) are dropped.
extern LogLevel currentLogLevel;

// printf-style logger. msg is formatted with params just like printf, and a
// trailing '\n' is always appended.
void log(LogLevel level, const char* msg, ...);
