
#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <Arduino.h>

#include "types.h"
#include "crc32.h"

byte XOFF = 0x13;
byte XON  = 0x12;

class SerialConnection {
  private:
    byte *_buffer;
    uint _bufferlen;
    long unsigned int _baud;
    unsigned int _timeout;
    HardwareSerial &_serial; 
  public:
    SerialConnection(HardwareSerial &serial,
                     byte *buffer,
                     const uint bufferlen,
                     const long unsigned int baud,
                     const unsigned int timeout): _serial(serial) {
      _buffer = buffer;
      _bufferlen = bufferlen;
      _baud = baud;
      _timeout = timeout;
    };
    SerialConnection(HardwareSerial &serial,
                     const uint bufferlen,
                     const long unsigned int baud,
                     const unsigned int timeout): _serial(serial) {
      _buffer = new byte[bufferlen];
      _bufferlen = bufferlen;
      _baud = baud;
      _timeout = timeout;
    };
    void initialize() {
      _serial.begin(_baud);
      _serial.setTimeout(_timeout);
    };
    bool read(byte* &buffer, uint &length) {
      uint readlen = 0;
      readlen = _serial.readBytes(_buffer, _bufferlen);
      if (readlen > 0) {
        console.printf(" Serial read: %d bytes (CRC: %ul)\n",readlen,crc32(_buffer, readlen));
        length = readlen;
        buffer = _buffer;
        return true;
      }
      return false;
    };
    void write(const byte *buffer, const uint length) {
      console.printf("Serial write: %u bytes (CRC: %ul)\n",length,crc32(buffer, length));
      _serial.write(buffer, length);
    };
    void xon() {
      _serial.write(&XON,1);
    };
    void xoff() {
      _serial.write(&XOFF,1);
    };
};
#endif
