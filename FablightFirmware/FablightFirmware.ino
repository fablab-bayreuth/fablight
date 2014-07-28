
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



