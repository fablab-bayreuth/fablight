
#ifndef FADE_H
#define FADE_H

#include "global.h"

extern unsigned long startTime;
extern uint8_t startColor[4];

struct Action {
    uint16_t fadeTime;
    int16_t color[4];
    unsigned relative: 
    1;
};

extern Action action[2];

extern int8_t actionIndex;

#endif // FADE_H
