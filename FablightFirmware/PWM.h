#ifndef PWM_H
#define PWM_H
#include <Arduino.h>
#include "global.h"

void set_red(uint16_t value);
void set_green(uint16_t value);
void set_blue(uint16_t value);
void set_white(uint16_t value);
void set_red_rel(int16_t diff);
void set_green_rel(int16_t diff);
void set_blue_rel(int16_t diff);
void set_white_rel(int16_t diff);

void setPWM(uint8_t color, uint16_t value);
void setPWM_rel(uint8_t color, int16_t diff);
void setPWMs(uint16_t white, uint16_t red, uint16_t green, uint16_t blue);
void setPWMs_rel(int16_t white, int16_t red, int16_t green, int16_t blue);
void initPWM();

#endif
