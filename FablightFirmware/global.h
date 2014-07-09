#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdint.h>
#define PIN_W 3
#define PIN_B 9
#define PIN_G 10
#define PIN_R 11

typedef enum {
  WHITE = 0,
  RED   = 1,
  GREEN = 2,
  BLUE  = 3
} color_t;

extern uint8_t colorToPin[4];
#endif
