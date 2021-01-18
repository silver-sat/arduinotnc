
//
// Suport for blinking the board's LED, no use of delay!!!
//

#ifndef _BLINKER_H_
#define _BLINKER_H_

#define LEDPIN LED_BUILTIN
#define LEDINT 100

timestamp _blinker_transtime = 0;
byte _blinker_ledstate = LOW;
byte _blinker_ledcount = 0;

void blinker_setup() {
  // Setup the blinking LED
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
}

void blinkit(byte count = 1) {
  _blinker_ledcount += count;
  if (_blinker_transtime == 0) {
    _blinker_ledstate = HIGH;
    digitalWrite(LEDPIN, _blinker_ledstate);
    _blinker_transtime = (millis() + LEDINT);
  }
}

void blinker_update() {
  if (_blinker_transtime > 0 && millis() > _blinker_transtime) {
    if (_blinker_ledstate == HIGH) {
      _blinker_ledstate = LOW;
      digitalWrite(LEDPIN, _blinker_ledstate);
      _blinker_ledcount--;
      if (_blinker_ledcount == 0) {
        _blinker_transtime = 0;
      } else {
        _blinker_transtime = (millis() + LEDINT);
      }
    } else {
      _blinker_ledstate = HIGH;
      digitalWrite(LEDPIN, _blinker_ledstate);
      _blinker_transtime = (millis() + LEDINT);
    }
  }
}

#endif
