
// Creates global blinker variable
#include "blinker.h"

// Creates global console variable for other classes to use. 
// output if CONSOLE_OUTPUT is true
#define CONSOLE_OUTPUT true
#include "console.h"

// Creates global variable nodeid;
#include "nodeid.h"

#include "types.h"
#include "params.h"

#include "kiss.h"
KISSFrames framer;

#include "serial.h"
byte rpiserialbuffer[RPIREADBUFFERSIZE];
SerialConnection rpi(
  RPISERIAL,
  rpiserialbuffer,
  RPIREADBUFFERSIZE,
  SERIALBAUD,
  SERIALTIMEOUT);

#include "radio.h"
byte rpiradiobuf[RPIWRITEBUFFERSIZE];
MultiMessageReliableRFM95Link radio(
  rpiradiobuf,
  RPIWRITEBUFFERSIZE);

//
// Initial setup
//

void setup() {

  // Initialize the console
  console.initialize();

  console.printf("Program: %s\n", __FILE__);
  console.printf("My ID %d, Other ID %d\n", nodeid.me(), nodeid.them());

  // initialze the serial connection with RPi
  rpi.initialize();

  // Setup the radio - specific to the type of radio link...
  radio.frequency(RADIO_FREQ);
  radio.power(RADIO_POWER);
  radio.receive_timeout(RADIO_RECEIVE_TIMEOUT);
  radio.send_delay(SEND_DELAY);
  radio.receive_delay(RECEIVE_DELAY);
  radio.send_ack_timeout(RADIO_SEND_ACK_TIMEOUT);
  radio.send_fail_delay(SEND_FAIL_DELAY);
  radio.max_attempts(MAX_ATTEMPTS);
  radio.message_size(MSGSIZE);
  radio.initialize();

  // Blink the LED three times
  blinker.blink(3);

}

//
// Repeating loop
//

// local variables for main loop
byte *rpireadbuf;
uint rpireadlen;

// local variables for main loop
byte *rpiwritebuf;
uint rpiwritelen;

// track frames to send
bool datatosend = false;
uint frameindex = 0;

void loop() {
  blinker.update();

  if (radio.receive(rpiwritebuf, rpiwritelen)) {
    rpi.write(rpiwritebuf, rpiwritelen);
  }

  if (datatosend) {
    if (!radio.busy()) {
      if (frameindex < framer.count()) {
        radio.send(rpireadbuf + framer.start(frameindex), framer.length(frameindex));
        frameindex += 1;
      } else {
        datatosend = false;
      }
    }
  } else {
    if (rpi.read(rpireadbuf, rpireadlen)) {
      if (framer.findframes(rpireadbuf, rpireadlen)) {
        datatosend = true;
        frameindex = 0;
      }
    }
  }
}
