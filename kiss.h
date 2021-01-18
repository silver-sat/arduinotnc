
//
// Support for chopping a buffer into individual KISS frames
//

// Relies on console, types, and MAX_KISS_FRAME

#include <assert.h>

#define MAXFRAMES 20
uint _kiss_framestart[MAXFRAMES];
uint _kiss_framelength[MAXFRAMES];
uint _kiss_framecount = 0;

// KISS frame start/end marker
#define FEND 0xC0

bool kissfindframes(const byte *buffer, unsigned int length) {
  unsigned int frstart = 0;
  unsigned int frend, frlen;
  unsigned int frameno = 0;
  // We assume the buffer has FEND at the beginning...
  while (frstart < length) {
    // find next FEND marker for the end of frame
    for (frend = (frstart + 1); frend < length; frend++) {
      if (buffer[frend] == FEND) {
        break;
      }
    }
    frend = min((frend + 1), length);
    frlen = frend - frstart;

    // check the frame has FEND at beginning and end
    // and is a data-frame and is not too long
    if ((frlen <= MAX_KISS_FRAME) &&
        (buffer[frstart] == FEND) &&
        ((buffer[frstart + 1] & 0x0F) == 0) &&
        (buffer[frend - 1] == FEND)) {
      // record the start and length of the frame
      _kiss_framestart[frameno] = frstart;
      _kiss_framelength[frameno] = frlen;
      frameno += 1;
      console("Frame %d: start %d %0x %0x len %d %0x\n", frameno, frstart, buffer[frstart], (buffer[frstart + 1] & 0x0F), frlen, buffer[frend - 1]);
    } else {
      console("Frame X: start %d %0x %0x len %d %0x\n", frstart, buffer[frstart], (buffer[frstart + 1] & 0x0F), frlen, buffer[frend - 1]);
    }
    // find next (start) FEND (starting with previous end FEND) 
    // that is not followed by a FEND 
    frstart = frend;
    while ((frstart < length) && (buffer[frstart + 1] == FEND)) {
      frstart += 1;
    }
  }
  _kiss_framecount = frameno;
  // for (uint i = 0; i < _kiss_framecount; i++) {
  //   console("Frame %d: start %d %0x %0x len %d %0x\n", i + 1, _kiss_framestart[i], buffer[_kiss_framestart[i]], (buffer[_kiss_framestart[i] + 1] & 0x0F), _kiss_framelength[i], buffer[_kiss_framestart[i] + _kiss_framelength[i] - 1]);
  // }
  return (_kiss_framecount > 0);
}

bool kissoneframe(byte *buffer, unsigned int length) {
  _kiss_framestart[0] = 0;
  _kiss_framelength[0] = length;
  _kiss_framecount = 1;
  if (buffer[length-1] != FEND) {
    buffer[length] = FEND;
    _kiss_framelength[0] += 1;
  }
  return true;
}

uint kissframestart(byte i) {
  assert(i < _kiss_framecount);
  return _kiss_framestart[i];
}

uint kissframelength(byte i) {
  assert(i < _kiss_framecount);
  return _kiss_framelength[i];
}

uint kissframecount() {
  return _kiss_framecount;
}
