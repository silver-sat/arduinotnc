
#ifndef _TIMER_H_
#define _TIMER_H_

#include <limits.h>

// Timing state variables
ulong _timer_nextrpiread = 0;
ulong _timer_nextsend = 0;
ulong _timer_nextxoff = 0;

#define FOREVER ULONG_MAX;

void timer_setup() {
  randomSeed(analogRead(0));
}

bool isafter(timestamp timepoint) {
  return (timepoint < millis());
}

ulong waitfor_atleast(timestamp current, duration period) {
  return max(current,(millis() + period));
}

ulong waitfor_atleast_random(timestamp current, duration period) {
  return max(current,(millis() + period + period*random(1024)/1024));
}

bool send_now() {
  return isafter(_timer_nextsend);
}

void send_waitfor_atleast(duration period) {
  _timer_nextsend = waitfor_atleast(_timer_nextsend, period);
}

void send_waitfor_atleast_random(duration period) {
  _timer_nextsend = waitfor_atleast_random(_timer_nextsend, period);
}

bool rpiread_now() {
  return isafter(_timer_nextrpiread);
}

void rpiread_waitfor_atleast(duration period) {
  _timer_nextrpiread = waitfor_atleast(_timer_nextrpiread, period);
}

void rpiread_waitfor_atleast_random(duration period) {
  _timer_nextrpiread = waitfor_atleast_random(_timer_nextrpiread, period);
}

bool xoff_now() {
  return isafter(_timer_nextxoff);
}

#endif
