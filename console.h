
/*
  console - library for easily generating formatted information to the Arduino serial console that can be switched off easily.

  // If output is true, then generate output to the console, otherwise no output
  SerialConsole console(output);

  ...

  // Use printf style formatting strings and variables...
  console("%s: %.3f %02x",string_value, float_value, byte_value);

*/

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <Arduino.h>

class SerialConsole {
  bool _output = false;
  char _outbuffer[256];

  public:
    SerialConsole(const bool output) {
      _output = output;
    }
    void initialize() {
      Serial.begin(9600);
    }
    void printf(const char* format, ...) {
      if (_output) {
        va_list args;
        va_start(args, format);
        vsprintf(_outbuffer, format, args);
        va_end(args);
        Serial.print(_outbuffer);
      }
    }
};

#endif
