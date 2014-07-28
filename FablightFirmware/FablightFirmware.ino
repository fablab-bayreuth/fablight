
#include <SoftwareSerial.h>

#include "global.h"
#include "ColorSpaces.h"
#include "PWM.h"
#include "FablightIR.h"
#include "Fade.h"

SoftwareSerial bluetooth(PIN_BT_TX, PIN_BT_RX);

//-------------------------------------------------------------------------

void setup() {
    initPWM();
    fablight_ir_init();
    Serial.begin(38400);
    bluetooth.begin(BT_BAUD);

    setPWMs(1, 1, 1, 1);
}


void loop() {
    handle_bluetooth();
    handle_ir();
    // doAction();
}


//-------------------------------------------------------------------------
// Bluetooth receive handler
// Bluetooth command format: "%04x%02x%02x%02x%02x", fadeTime, white, ref, green, blue
void handle_bluetooth() 
{
    static uint8_t index=0, err=0;
    static uint16_t  red=0, green=0, blue=0, white=0;

    while(bluetooth.available()) {
        char c = bluetooth.read();
        Serial.write(c);
        if(c == '\r' || c == '\n') { // line ending terminates command 
            Serial.write('\n');
            bluetooth.write('.');        
            if (!err && index==12)
                setPWMs(white, red, green, blue);
            index = 0;
            err = 0;
            continue;
        } else if (err || index > 12) {
            err = 1;
            continue;    
        } else { // hex ascii to integer
            if (c >= '0' && c <= '9') {
                c -= '0'; 
            } else if (c >= 'A' && c <= 'F') {
                c -= 'A' - 10;
            } else {
                err = 1;
                continue;
            }
            switch(index++) {
                case 0: case 1: case 2: case 3: break;
                case 4: white = c<<8; break;
                case 5: white |= c<<4; break;
                case 6: red = c<<8; break;
                case 7: red |= c<<4; break;
                case 8: green = c<<8; break;
                case 9: green |= c<<4; break;
                case 10: blue = c<<8; break;
                case 11: blue |= c<<4; break;
                default: err = 1; break;
            }
        }
    }
}

//--------------------------------------------------------------------------------

/*
// bluetooth handler with fade action
void handle_bluetooth() {
 	static int index = 0;
 
 	while(bluetooth.available()) {
 		char c = bluetooth.read();
 		Serial.write(c);
 		if(c == '\r') {
 			if(index == 12) {
 				actionIndex |= 1;
 			}
 			Serial.write('\n');
 			bluetooth.write('.');
 			index = 0;
 			continue;
 		} else {
 			if(c >= '0' && c <= '9') {
 				c -= '0';
 			} else if(c >= 'A' && c <= 'F') {
 				c -= 'A' - 10;
 			} else {
 				continue;
 			}
 		}
 		switch(index) {
 		case 0: action[actionIndex].fadeTime  = c << 12; break;
 		case 1: action[actionIndex].fadeTime |= c <<  8; break;
 		case 2: action[actionIndex].fadeTime |= c <<  4; break;
 		case 3: action[actionIndex].fadeTime |= c; break;
 		case 4: case 6: case 8: case 10:
 			action[actionIndex].color[index/2 - 2] =  c << 8; break;
 		case 5: case 7: case 9: case 11:
 			action[actionIndex].color[index/2 - 2] |= c << 4; break;
 		}
 		index++;
 	}
 }
 */

//-------------------------------------------------------------------------

void handle_ir(void)
{
    uint8_t button = fablight_ir_get_button();
    if (button==0) return;
    switch (button) {
    case 11:  setPWMs( 0, 0, 0, 0 ); break;
    case 12:  setPWMs( 1, 1, 1, 1 ); break;
    case 13:  setPWMs( 50, 50, 50, 50 ); break;
    case 21:  set_red_rel( 1 ); break;
    case 22:  set_red_rel( -1 ); break; 
    case 31:  set_green_rel( 1 ); break;
    case 32:  set_green_rel( -1 ); break;
    case 41:  set_blue_rel( 1 ); break;
    case 42:  set_blue_rel( -1 ); break;
    case 51:  set_white_rel( 1 ); break;
    case 52:  set_white_rel( -1 ); break;
    }
}

//-------------------------------------------------------------------------



