
/*
  timer - tracking the next time events should happen
  Created by Nathan Edwards 1/31/2021.
  Released into the public domain.
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#include <Arduino.h>

#include "types.h"

class Timer {

  private:

    static bool _rngseeded;
    timestamp _next = 0;

  public:

    Timer() {
      _next = 0;
      if (!_rngseeded) {
        randomSeed(analogRead(0));
        _rngseeded = true;
      }
    }

    bool ready() {
      return (_next < millis());
    }

    void wait(duration period) {
      _next = max(_next, (millis() + period));
    }

    void random_wait(duration period1, duration period2) {
      this->wait(period1 + (period2 - period1)*random(1024) / 1024);
    }

    void random_wait(duration period) {
      this->random_wait(period, 2*period);
    }
};

bool Timer::_rngseeded = false;

#endif
