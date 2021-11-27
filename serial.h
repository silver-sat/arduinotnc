
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
    long unsigned int _totalreadbytes;
    long unsigned int _totalreads;
    long unsigned int _totalwritebytes;
    long unsigned int _totalwrites;
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
      _totalreadbytes = 0;
      _totalreads = 0;
      _totalwritebytes = 0;
      _totalwrites = 0;
      };
    SerialConnection(HardwareSerial &serial,
                     const uint bufferlen,
                     const long unsigned int baud,
                     const unsigned int timeout): _serial(serial) {
      _buffer = new byte[bufferlen];
      _bufferlen = bufferlen;
      _baud = baud;
      _timeout = timeout;
      _totalreadbytes = 0;
      _totalreads = 0;
      _totalwritebytes = 0;
      _totalwrites = 0;
    };
    void initialize() {
      _serial.begin(_baud);
      _serial.setTimeout(_timeout);
      _totalreadbytes = 0;
      _totalreads = 0;
      _totalwritebytes = 0;
      _totalwrites = 0;
    };
    bool read(byte* &buffer, uint &length) {
      uint readlen = 0;
      readlen = _serial.readBytes(_buffer, _bufferlen);
      if (readlen > 0) {
        _totalreads += 1;
        _totalreadbytes += readlen;
#ifdef HAS_CONSOLE
        console.printf(" Serial read: %d bytes (CRC: %ul, Total: %ul/%ul)\n", readlen, crc32(_buffer, readlen),_totalreadbytes,_totalreads);
#endif
        length = readlen;
        buffer = _buffer;
        return true;
      }
      return false;
    };
    void write(const byte *buffer, const uint length) {
      _totalwrites += 1;
      _totalwritebytes += length;
#ifdef HAS_CONSOLE
      console.printf("Serial write: %u bytes (CRC: %ul, Total: %ul/%ul)\n", length, crc32(buffer, length),_totalwritebytes,_totalwrites);
#endif
      _serial.write(buffer, length);
    };
    void xon() {
      _serial.write(&XON, 1);
    };
    void xoff() {
      _serial.write(&XOFF, 1);
    };
};

#endif
