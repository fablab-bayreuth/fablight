#include <SoftwareSerial.h>
#include "global.h"
#include "ColorSpaces.h"
#include "PWM.h"
#include "FablightIR.h"

SoftwareSerial bt(PIN_BT_TX, PIN_BT_RX);

void setup() {
	initPWM();
	fablight_ir_init();
	Serial.begin(38400);
	bt.begin(BT_BAUD);
	
	setPWM(RED, 0);
	setPWM(GREEN, 0);
	setPWM(BLUE, 0);
	setPWM(WHITE, 0);
}

unsigned long startTime = 0;
uint8_t startColor[4] = { 0, 0, 0, 0 };

struct action {
	uint16_t fadeTime;
	int16_t color[4];
        unsigned relative: 1;
} action[2];

int8_t actionIndex = 0;

void loop() {
	handle_bluetooth();
	handle_ir();
	doAction();
}

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
		} else {
			for(uint8_t i = 0; i < 4; i++) {
				uint16_t value;
				if(startColor[i] > action[0].color[i]) {
					value = startColor[i] - ((startColor[i] - action[0].color[i]) * t) / action[0].fadeTime;
				} else {
					value = startColor[i] + ((action[0].color[i] - startColor[i]) * t) / action[0].fadeTime;
				}
				setPWM(i, value);
			}
		}
	} else if(actionIndex == 1) {
		if (action[0].relative) { // Relative change: Calculate new absolute before starting action
			for(uint8_t i = 0; i < 4; i++) {
		        action[0].color[i] += startColor[i];
		    }
		}
		startTime = millis();
		doing = 1;
	}
}

void handle_bluetooth() {
	static int index = 0;

	while(bt.available()) {
		char c = bt.read();
		Serial.write(c);
		if(c == '\r') {
			if(index == 12) {
				actionIndex |= 1;
			}
			Serial.write('\n');
			bt.write('.');
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


void handle_ir(void)
{
    uint8_t button = fablight_ir_get_button();
    if (button==0) return;
    switch (button) {
      case 11:
        action[actionIndex].fadeTime = 0;
        action[actionIndex].color[RED] = 0;
        action[actionIndex].color[GREEN] = 0;
        action[actionIndex].color[BLUE] = 0;
        action[actionIndex].color[WHITE] = 10;
        actionIndex |= 1;
        break;
      case 21:
        action[actionIndex].fadeTime = 0;
        action[actionIndex].color[RED] = 0;
        action[actionIndex].color[GREEN] = 0;
        action[actionIndex].color[BLUE] = 0;
        action[actionIndex].color[WHITE] = 0;
        actionIndex |= 1;
        break;
    }
    
}



