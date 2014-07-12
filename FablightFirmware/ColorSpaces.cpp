#include "ColorSpaces.h"
#include "global.h"

void rgb_to_hsv( int16_t red, int16_t green, int16_t blue, int16_t *hue, int16_t *sat, int16_t *val )
{
  int16_t  maxi, mini, contr;
 
  // maxi= max(r,g,b), mini=min(r,g,b)  :TODO: may be optimized and combined with switches below
  maxi = red; if(green>maxi) maxi=green; if (blue>maxi) maxi=blue;	
  mini = red; if(green<mini) mini=green; if (blue<mini) mini=blue;	
 
  // Value (brightness)
  *val = maxi;
  if (maxi == 0) return; // black: hue and sat undefined 
  
  // Saturation
  contr = maxi-mini;
  *sat = contr/maxi;
  if (contr == 0) return; // grey: hue undefined

  // Hue (color), 12 bit resolution
  if (maxi == red) 
    *hue = 0 + 682*(green-blue)/contr;
  else if (maxi == green)
    *hue = 1365 + 682*(blue-red)/contr;
  else
    *hue = 2730 + 682*(red-green)/contr;
  if (*hue<0)
    *hue += 4096;

  return;
}


void hsv_to_rgb( int16_t hue, int16_t sat, int16_t val, int16_t *red, int16_t *green, int16_t *blue)
{ 
  if (sat==0) {
    *red = *green = *blue = val;
   return; 
  }
  
  uint8_t  hi = hue/683; // assuming 12 bit resolution
  uint8_t  f = (hue - (hi * 683)) * 6; // remainder scaled to 12 bit
  #define  P  (val * (4096 - sat)) >> 12
  #define  Q  (val * (4096 - ((sat * f) >> 12))) >> 12
  #define  T  (val * (4096 - ((sat * (4096 - f)) >> 12))) >> 12
  
  switch ( hi ) {
    case 0:
      *red = val; *green = T; *blue = P;
      return;
    case 1:
      *red = Q; *green = val; *blue = P;
      return;
    case 2:
      *red = P; *green = val; *blue = T;
      return;
    case 3:
      *red = P; *green = Q; *blue = val;
      return;
    case 4:
      *red = T; *green = P; *blue = val;
      return;
    case 5:
      *red = val; *green = P; *blue = Q;
      return;
    default: // invalid hue
      *red = *green = *blue = 0;
      return;
  }
  
}


