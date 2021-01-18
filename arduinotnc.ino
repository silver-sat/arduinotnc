
#include "types.h"
#include "params.h"
#include "globals.h"
#include "console.h"
#include "serial.h"
#include "blinker.h"
#include "nodeid.h"
#include "radio.h"
#include "kiss.h"
#include "timer.h"

//
// Initial setup
//

void setup() {

  // Start the console
  console_setup(CONSOLE);

  console("Program: %s\n", __FILE__);

  // Setup the blinker
  blinker_setup();

  // Setup timing variables
  timer_setup();

  // Setup my nodeid and the other nodeid
  nodeid_setup();

  // Setup the RPi serial connection
  serial_setup(rpireadbuf, RPIREADBUFFERSIZE);

  // Setup the radio
  radio_setup();

  // Blink the LED three times
  blinkit(3);
}

//
// Repeating loop
//

// for tracking send information relative to current rpireadbuf...
bool havedata = false;
uint frameindex = 0;
uint msgindex = 0;
uint msgattempts = 0;

// total frames sent overall...
uint totalframes = 1;

// tracking details of received radio data...
byte currentframeid = 0;
byte currentframemsgcount = 0;
byte nextmsgindex = 0;

void loop() {
  blinker_update();

  if (radio_receive()) {
    bool first = (radiobuf[0] == FEND);
    bool last = (radiobuf[radiolen - 1] == FEND);
    byte frameid = (radioflags >> 2) & 0x03;
    byte msgindex = (radioflags & 0x03);
    // console("len %d radioflags: %d%d%d%d\n", radiolen, ((radioflags >> 3) & 0x01), ((radioflags >> 2) & 0x01), ((radioflags >> 1) & 0x01), radioflags & 0x01);
    console("Received %d-%d (%d-%d) len %d first %d last %d\n", frameid, msgindex, currentframeid, nextmsgindex, radiolen, 1 * first, 1 * last);
    if (first || currentframeid != frameid) {
      if (currentframemsgcount > 0 || currentframeid != 0) {
        console("Bad msg dropped, frame id %d to be dropped\n", currentframeid);
      }
      currentframeid = frameid;
      currentframemsgcount = 0;
      nextmsgindex = 0;
    }
    if (frameid == currentframeid && msgindex == nextmsgindex) {
      console("Copying frame id %d msg index %d to rpiwritebuf\n", frameid, msgindex);
      memcpy(rpiwritebuf + MSGSIZE * currentframemsgcount, radiobuf, radiolen);
      if (last) {
        serial_write(rpiwritebuf, MSGSIZE * currentframemsgcount + radiolen);
        currentframeid = 0; // have to get the first one to set the frame
        nextmsgindex = 0;
      } else {
        nextmsgindex += 1;
        nextmsgindex &= 0x03;
        currentframemsgcount += 1;
      }
    } else {
      console("Bad msg dropped, frame id %d to be dropped\n", currentframeid);
      currentframeid = 0;
      nextmsgindex = 0;
    }

    // Delay any other action(s) to give a chance for more radio messages
    send_waitfor_atleast(RECEIVE_SEND_DELAY);
    rpiread_waitfor_atleast(RECEIVE_SEND_DELAY);
  }

  if (rpiread_now() && !havedata && serial_read(&rpireadlen)) {
    if (kissfindframes(rpireadbuf, rpireadlen)) {
      send_waitfor_atleast(SEND_DELAY);
      havedata = true;
      frameindex = 0;
      msgindex = 0;
      msgattempts = 0;
    }
  }

  if (havedata && send_now()) {
    uint msgstart = msgindex * MSGSIZE;
    uint msglen = kissframelength(frameindex) - msgstart;
    bool last = true;
    if (msglen > MSGSIZE) {
      msglen = MSGSIZE;
      last = false;
    }
    byte frameid = ((totalframes + frameindex) & 0x03);
    console("About to send: Frame %d (%d) msg %d len %d attempt %d\n", frameindex + 1, frameid, msgindex + 1, msglen, msgattempts + 1);
    byte flags = (frameid << 2) | (msgindex & 0x03);
    // console("flags: %d%d%d%d\n",((flags>>3)&0x01),((flags>>2)&0x01),((flags>>1)&0x01),flags&0x01);
    if (radio_send(rpireadbuf + kissframestart(frameindex) + msgstart, msglen, flags)) {
      if (last) {
        frameindex += 1;
        if (frameindex >= kissframecount()) {
          totalframes += kissframecount();
          havedata = false;
        } else {
          msgindex = 0;
          msgattempts = 0;
        }
        // end of frame, potentially have a longer delay to allow receipt of IP acknowledgement frame 
        send_waitfor_atleast(INTERFRAME_DELAY);
      } else {
        msgindex += 1;
        msgattempts = 0;
        // time between successive sends (in-frame)
        send_waitfor_atleast(SEND_DELAY);
      }
    } else {
      msgattempts += 1;
      if (msgattempts >= MAX_ATTEMPTS) {
        frameindex += 1;
        if (frameindex >= kissframecount()) {
          totalframes += kissframecount();
          havedata = false;
        } else { 
          msgindex = 0;
          msgattempts = 0;
        }
      }
      // Exponentially longer random waiting due to failures...
      send_waitfor_atleast_random(SEND_FAIL_DELAY*pow(2, msgattempts));
    }
  }
}
