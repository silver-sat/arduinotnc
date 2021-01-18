
#ifndef _NODEID_H_
#define _NODEID_H_

#include <EEPROM.h>

byte _nodeid_mynodeid;
byte _nodeid_othernodeid;

void nodeid_setup() {
  char tag[7];
  for (int i = 0; i < 6; i++) {
    tag[i] = EEPROM.read(i);
  }
  tag[6] = '\0';

  _nodeid_mynodeid = atoi(tag + 3);
  _nodeid_othernodeid = (3 - _nodeid_mynodeid);
  console("My ID %d, Other ID %d\n", _nodeid_mynodeid, _nodeid_othernodeid);
}

byte mynodeid() {
  return _nodeid_mynodeid;
}

byte othernodeid() {
  return _nodeid_othernodeid;
}

#endif
