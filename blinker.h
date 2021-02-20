
/*
  blinker - library for flashing the Arduino builtin LED, without using delay

  Blinker blinker;

  void loop() {
    // Must be called in every loop iteration....
    blincker.update();

    if (...) {
      // blink the built-in LED three times...
      blinker.blinkit(3);  
    }
  }
  
*/

#ifndef _BLINKER_H_
#define _BLINKER_H_

#include <Arduino.h>

#define LEDPIN LED_BUILTIN
#define LEDINT 100

class Blinker {

  private:
    unsigned int _ledint;
    unsigned int _ledpin;
    unsigned long int _transtime;
    unsigned char _ledstate;
    unsigned int _ledcount;

  public:
    Blinker(const unsigned int pin = LEDPIN, const unsigned int interval = LEDINT) {
      _ledpin = pin;
      _ledint = interval;
      _transtime = 0;
      _ledstate = LOW;
      _ledcount = 0;
      pinMode(_ledpin, OUTPUT);
      digitalWrite(_ledpin, LOW);
    }
    void update() {
      if (_transtime > 0 && millis() > _transtime) {
        if (_ledstate == HIGH) {
          _ledstate = LOW;
          digitalWrite(_ledpin, _ledstate);
          _ledcount--;
          if (_ledcount == 0) {
            _transtime = 0;
          } else {
            _transtime = (millis() + _ledint);
          }
        } else {
          _ledstate = HIGH;
          digitalWrite(_ledpin, _ledstate);
          _transtime = (millis() + _ledint);
        }
      }

    }
    void blink(const unsigned int count = 1) {
      _ledcount += count;
      if (_transtime == 0 && _ledstate == LOW) {
        _ledstate = HIGH;
        digitalWrite(_ledpin, _ledstate);
        _transtime = (millis() + _ledint);
      }
    }
};

#endif
