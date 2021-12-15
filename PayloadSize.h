#ifndef PAYLOAD_SIZE_H
#define PAYLOAD_SIZE_H

#include <stdint.h>

enum PayloadSize : uint8_t {
  COMMAND = 0,
  BYTE = 1,
  SHORT = 2,
  INT = 4
};

#endif