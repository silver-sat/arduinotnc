
// buffers for rpi read, rpi write
byte *_serial_buffer;
uint _serial_buflen;

void serial_setup(byte *buffer, uint bufferlen) {
	RPISERIAL.begin(SERIALBAUD);
	RPISERIAL.setTimeout(SERIALTIMEOUT);
  _serial_buffer = buffer;
  _serial_buflen = bufferlen;
}

bool serial_read(uint* outlen) {
  uint readlen = RPISERIAL.readBytes(_serial_buffer, _serial_buflen);
  if (readlen > 0) {
    console(" Serial read: %d bytes\n",readlen);
    (*outlen) = readlen;
    return true;
	}
	return false;
}

void serial_write(const byte *buffer, uint length) {
	console("Serial write: %u bytes\n",length);
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
