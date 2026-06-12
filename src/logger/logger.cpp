#include <stdarg.h>

#include "logger.h"

LogLevel currentLogLevel = DEBUG;

static const char* levelTag(LogLevel level) {
  switch (level) {
  case ERROR:
    return "[ERROR] ";
  case WARN:
    return "[WARN] ";
  case INFO:
    return "[INFO] ";
  case DEBUG:
    return "[DEBUG] ";
  }
  return "";
}

void log(LogLevel level, const char* msg, ...) {
  if (level > currentLogLevel) {
    return;
  }

  char buffer[256];
  va_list args;
  va_start(args, msg);
  vsnprintf(buffer, sizeof(buffer), msg, args);
  va_end(args);

  Serial.print(levelTag(level));
  Serial.print(buffer);
  Serial.print('\n');
}
