#ifndef PWM_H
#define PWM_H
#include <Arduino.h>
#include "global.h"

void setPWM(uint8_t color, uint16_t value);
void initPWM();

#endif
