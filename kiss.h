
//
// Support for chopping a buffer into individual KISS frames
//

#ifndef _KISS_H_
#define _KISS_H_

#include <assert.h>

#define MAXFRAMES 20

// KISS frame start/end marker
#define FEND 0xC0

class KISSFrames {
  protected:
    uint _start[MAXFRAMES];
    uint _length[MAXFRAMES];
    uint _count;

  public:
    bool findframes(byte *buffer, unsigned int length) {
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
          _start[frameno] = frstart;
          _length[frameno] = frlen;
          frameno += 1;
          console.printf("Frame %d: start %d %0x %0x len %d %0x\n", frameno, frstart, buffer[frstart], (buffer[frstart + 1] & 0x0F), frlen, buffer[frend - 1]);
        } else {
          console.printf("Frame X: start %d %0x %0x len %d %0x\n", frstart, buffer[frstart], (buffer[frstart + 1] & 0x0F), frlen, buffer[frend - 1]);
        }
        // find next (start) FEND (starting with previous end FEND)
        // that is not followed by a FEND
        frstart = frend;
        while ((frstart < length) && (buffer[frstart + 1] == FEND)) {
          frstart += 1;
        }
      }
      _count = frameno;
      return (_count > 0);
    };
    uint start(byte i) {
      assert(i < _count);
      return _start[i];
    };
    uint length(byte i) {
      assert(i < _count);
      return _length[i];
    };
    uint count() {
      return _count;
    };
};

class OneFrame: KISSFrames {
    bool findframes(byte * buffer, unsigned int length) {
      _start[0] = 0;
      _length[0] = length;
      _count = 1;
      if (buffer[length - 1] != FEND) {
        buffer[length] = FEND;
        _length[0] += 1;
      }
      return true;
    };
};

#endif
