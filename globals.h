
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "types.h"
#include "params.h"

#include "blinker.h"
Blinker blinker;

#include "console.h"
SerialConsole console(CONSOLE);

#include "nodeid.h"
NodeID nodeid;

#include "timer.h"
Timer send;

#include "serial.h"
byte rpiserialbuffer[RPIREADBUFFERSIZE];
SerialConnection rpi(RPISERIAL,
                     rpiserialbuffer,
                     RPIREADBUFFERSIZE,
                     SERIALBAUD,
                     SERIALTIMEOUT);

#include "kiss.h"
KISSFrames kiss;

#define RPIWRITEBUFFERSIZE MAX_KISS_FRAME
byte rpiwritebuf[RPIWRITEBUFFERSIZE];

#endif
