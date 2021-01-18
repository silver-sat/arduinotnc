
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define RPIREADBUFFERSIZE 4096
byte rpireadbuf[RPIREADBUFFERSIZE];
uint rpireadlen;

#define RPIWRITEBUFFERSIZE MAX_KISS_FRAME
byte rpiwritebuf[RPIWRITEBUFFERSIZE];

#endif
