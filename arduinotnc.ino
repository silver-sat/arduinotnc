
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
MultiMessageRadioLink radio(
  rpiradiobuf,
  RPIWRITEBUFFERSIZE);

//
// Initial setup
//

void setup() {

  // Initialize the console
#ifdef HAS_CONSOLE
  console.initialize();
#endif

#ifdef HAS_CONSOLE
  console.printf("Program: %s\n", __FILE__);
#ifdef HAS_NODEID
  console.printf("My ID %d, Other ID %d\n", nodeid.me(), nodeid.them());
#endif
#endif

  // initialze the serial connection with RPi
  rpi.initialize();

  // Setup the radio - specific to the type of radio link...
  RFM95_RadioLink *link = new RFM95_RadioLink();
  link->frequency(RADIO_FREQ);
  link->power(RADIO_POWER);
  link->receive_timeout(RADIO_RECEIVE_TIMEOUT);
  link->initialize();
  
  UnreliableDatagramLink *datagram = new UnreliableDatagramLink();
  datagram->initialize(*link);

  // ReliableDatagramLink *datagram = new ReliableDatagramLink();
  // datagram->send_ack_timeout(RADIO_SEND_ACK_TIMEOUT);
  // datagram->initialize(*link);
  
  radio.send_delay(SEND_DELAY);
  radio.receive_delay(RECEIVE_DELAY);
  // radio.send_fail_delay(SEND_FAIL_DELAY);
  // radio.max_attempts(MAX_ATTEMPTS);
  radio.last_msg_delay(INTER_FRAME_DELAY);
  radio.message_size(MSGSIZE);
  radio.initialize(*datagram);

#ifdef HAS_BLINKER
  // Blink the LED three times
  blinker.blink(3);
#endif

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
  
#ifdef HAS_BLINKER
  blinker.update();
#endif

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
