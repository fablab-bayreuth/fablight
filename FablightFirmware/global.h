#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdint.h>
#define PIN_W 3
#define PIN_B 9
#define PIN_G 10
#define PIN_R 11
#define PIN_IR 5 // Infrared receiver

#define PIN_BT_KEY 8
#define PIN_BT_RX 13
#define PIN_BT_TX 12
#define BT_BAUD 9600

typedef enum {
  WHITE = 0,
  RED   = 1,
  GREEN = 2,
  BLUE  = 3
} color_t;

extern uint8_t colorToPin[4];
#endif // GLOBAL_H
