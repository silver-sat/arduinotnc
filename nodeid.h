
#ifndef _NODEID_H_
#define _NODEID_H_

#include <Arduino.h>
#include <EEPROM.h>
#include "types.h"

class NodeID {
  private:
    byte _me;
    byte _them;
  public:
    NodeID() {
      char tag[7];
      for (int i = 0; i < 6; i++) {
        tag[i] = EEPROM.read(i);
      }
      tag[6] = '\0';
      _me = atoi(tag + 3);
      _them = (3 - _me);
    }
    byte me() {
      return _me;
    }
    byte them() {
      return _them;
    }
    void write_nodeid(byte id) {
      char tag[7];
      sprintf(tag, "ARD%03d", id);
      for (int i = 0; i < 6; i++) {
        EEPROM.write(i, tag[i]);
      }
    }
};

NodeID nodeid;

#endif
