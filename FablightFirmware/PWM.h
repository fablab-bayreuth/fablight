#ifndef PWM_H
#define PWM_H
#include <Arduino.h>
#include "global.h"

void setPWM(uint8_t color, uint16_t value);
void setPWMs(uint16_t white, uint16_t red, uint16_t green, uint16_t blue);
void initPWM();

#endif
