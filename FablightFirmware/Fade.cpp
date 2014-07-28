#include <Arduino.h>
#include "PWM.h"
#include "Fade.h"


//--------------------------------------------------------------------------------

unsigned long startTime = 0;
uint8_t startColor[4] = { 
    0, 0, 0, 0 };

Action action[2];

int8_t actionIndex = 0;

//-----------------------------------------------------------------------------------

void action_fade_abs( int16_t red, int16_t green = 0, int16_t blue = 0, int16_t white = 0, uint16_t fadeTime = 0)
{
    action[actionIndex].fadeTime = fadeTime;
    action[actionIndex].color[RED] = red;
    action[actionIndex].color[GREEN] = green;
    action[actionIndex].color[BLUE] = blue;
    action[actionIndex].color[WHITE] = white;
    action[actionIndex].relative = 0;  
    actionIndex |= 1;
}

//-----------------------------------------------------------------------------------

void action_fade_rel( int16_t red, int16_t green = 0, int16_t blue = 0, int16_t white = 0, uint16_t fadeTime = 0)
{
    action[actionIndex].fadeTime = fadeTime;
    action[actionIndex].color[RED] = red;
    action[actionIndex].color[GREEN] = green;
    action[actionIndex].color[BLUE] = blue;
    action[actionIndex].color[WHITE] = white;
    action[actionIndex].relative = 1;    
    actionIndex |= 1;
}

//-----------------------------------------------------------------------------------

void doAction() {
    static int8_t doing = 0;
    if(doing == 1) {
        uint16_t t = millis() - startTime;
        if(action[0].fadeTime == 0 || t >= action[0].fadeTime) {
            doing = 0;
            for(uint8_t i = 0; i < 4; i++) {
                setPWM(i, (uint16_t)action[0].color[i]);
                startColor[i] = action[0].color[i];
            }
            action[0] = action[1];
            actionIndex = 0;
        } 
        else {
            for(uint8_t i = 0; i < 4; i++) {
                uint16_t value;
                if(startColor[i] > action[0].color[i]) {
                    value = startColor[i] - ((startColor[i] - action[0].color[i]) * t) / action[0].fadeTime;
                } 
                else {
                    value = startColor[i] + ((action[0].color[i] - startColor[i]) * t) / action[0].fadeTime;
                }
                setPWM(i, value);
            }
        }
    } 
    else if(actionIndex == 1) {
        if (action[0].relative) { // Relative change: Calculate new absolute before starting action
            for(uint8_t i = 0; i < 4; i++) {
                action[0].color[i] += startColor[i];
            }
        }
        startTime = millis();
        doing = 1;
    }
}

