//
// Support for computing checksums
//

#ifndef _CRC32_H_
#define _CRC32_H_

#include <assert.h>

unsigned long crc32(const uint8_t *buffer, const int len) {
  int i, j;
  unsigned long byte, crc, mask;

  i = 0;
  crc = 0xFFFFFFFF;
  while (i < len) {
    byte = buffer[i];            // Get next byte.
    crc = crc ^ byte;
    for (j = 7; j >= 0; j--) {    // Do eight times.
      mask = -(crc & 1);
      crc = (crc >> 1) ^ (0xEDB88320 & mask);
    }
    i = i + 1;
  }
  return ~crc;
}

uint8_t getbyte(unsigned long a, uint8_t b) {
  return (uint8_t)((a >> (b * 8)) & 0x000000ff);
}

uint8_t crc32byte(const uint8_t *buffer, const int len, const uint8_t index) {
  assert (index <= 3);
  unsigned long crc = crc32(buffer, len);
  return getbyte(crc, index);
}

#endif
