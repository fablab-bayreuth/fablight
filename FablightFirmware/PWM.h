#ifndef PWM_H
#define PWM_H
#include <Arduino.h>
#include "global.h"

// Single color setters
void set_red(uint16_t value);
void set_green(uint16_t value);
void set_blue(uint16_t value);
void set_white(uint16_t value);

// Relative (additive) change
void set_red_rel(int16_t diff);
void set_green_rel(int16_t diff);
void set_blue_rel(int16_t diff);
void set_white_rel(int16_t diff);

// Relative (by factor) change
void set_red_fact( float fact );
void set_green_fact( float fact );
void set_blue_fact( float fact );
void set_white_fact( float fact );

// Single color getters
uint16_t get_red(void);
uint16_t get_green(void);
uint16_t get_blue(void);
uint16_t get_white(void);

void setPWM(uint8_t color, uint16_t value);
void setPWM_rel(uint8_t color, int16_t diff);
void setPWMs(uint16_t red, uint16_t green, uint16_t blue, uint16_t white);
void setPWMs_rel(int16_t red, int16_t green, int16_t blue, uint16_t white);
void initPWM();

#endif
