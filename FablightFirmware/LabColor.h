#ifndef LABCOLOR_H
#define LABCOLOR_H

#include "global.h"
#include <stdint.h>

void labToRgb(int8_t l, int8_t a, int8_t b, uint8_t out[4]);
void rgb_to_hsv( int16_t red, int16_t green, int16_t blue, int16_t *hue, int16_t *sat, int16_t *val);
void hsv_to_rgb( int16_t hue, int16_t sat, int16_t val, int16_t *red, int16_t *green, int16_t *blue);

#endif // LABCOLOR_H
