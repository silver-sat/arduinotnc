
//
// Support for switchable console output and printf formatting
//

bool _console_output = false;
char _console_outbuffer[128];

void console_setup(bool output) {
  if (output) {
    Serial.begin(9600);
  }
}

// This is a c-preprocessor #define to simplify things...

#define console(...) { if (_console_output) { sprintf(_console_outbuffer, __VA_ARGS__); Serial.print(_console_outbuffer); }; };
