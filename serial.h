
#ifndef _SERIAL_H_
#define _SERIAL_H_

// buffers for rpi read, rpi write
byte *_serial_buffer=0;
uint _serial_buflen=0;

void serial_setup(byte *buffer, uint bufferlen) {
	RPISERIAL.begin(SERIALBAUD);
	RPISERIAL.setTimeout(SERIALTIMEOUT);
  _serial_buffer = buffer;
  _serial_buflen = bufferlen;
}

bool serial_read(uint* outlen) {
  uint readlen = 0;
  readlen = RPISERIAL.readBytes(_serial_buffer, _serial_buflen);
  if (readlen > 0) {
    console(" Serial read: %d bytes (CRC: %ul)\n",readlen,crc32(_serial_buffer, readlen));
    (*outlen) = readlen;
    return true;
	}
	return false;
}

void serial_write(const byte *buffer, uint length) {
	console("Serial write: %u bytes (CRC: %ul)\n",length,crc32(buffer, length));
	RPISERIAL.write(buffer, length);
}

byte XOFF = 0x13;
void serial_write_xoff() {
  RPISERIAL.write(&XOFF,1);
}

byte XON = 0x12;
void serial_write_xon() {
  RPISERIAL.write(&XON,1);
}

#endif
