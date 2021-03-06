
//-------------------------------------------------------------------------

#define FABLIGHT_ID  "FabLight0-proto"  // id (name) of this device

//-------------------------------------------------------------------------

#include <SoftwareSerial.h>
#include <EEPROM.h> // needs to be included here to be accessible

#include "global.h"
#include "ColorSpaces.h"
#include "PWM.h"
#include "FablightIR.h"
#include "Fade.h"
#include "Store.h"
#include "FablightUtil.h"

SoftwareSerial bluetooth(PIN_BT_TX, PIN_BT_RX);

//-------------------------------------------------------------------------

void setup() {
    initPWM();
    fablight_ir_init();
    Serial.begin(38400);
    bluetooth.begin(BT_BAUD);
    setPWMs(1, 1, 1, 1);
    Serial.println("Yabadabadoo!");
}


void loop() {
    handle_bluetooth();
    handle_ir();
    handle_serial();
    // doAction();
}


//-------------------------------------------------------------------------
// Serial receive handler
void handle_serial()
{
    static char cmd[20] = {0};
    static uint8_t  pc=0, err=0;
    
    while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\r' || c == '\n') {  // Line ending terminates command
            if (err) {  // Are we in arror state?
                err = 0; pc = 0;
                continue;
            } else {  // err != 0
                if (pc == 0)  // Ignore empty line
                    continue;  
                cmd[pc] = 0;
                eval_serial_cmd( strupr(cmd), pc );
                pc = 0;
                continue;
            }
        } else {  // Not a termchar        
            cmd[pc++] = c;
            if (pc >= sizeof(cmd)) { // Command buffer overflow
                err = 1; pc = 0;
                Serial.println("Error: Command buffer overflow");
                continue;
            }
        }  
    }  // end Serial.available
}



int8_t  eval_serial_cmd( const char* cmd, uint8_t len )
{
    uint16_t red=0, green=0, blue=0, white=0;
    
    if (!strncmp( cmd, "FABLIGHT?", 9 )) {
        Serial.println("YESSIR!");
    }
    
    else if (!strncmp( cmd, "ID?", 3 )) {
        Serial.println(FABLIGHT_ID);
    }
    
    else if (!strncmp( cmd, "R:", 2 )) {
        if (len<4) { Serial.println("Error: malformed command"); return 1; }
        red = hex_to_uint8(cmd+2)<<4;
        set_red(red);
        Serial.println("OK");        
        Serial.print("Via serial: R"); Serial.println(red);
    }
    
    else if (!strncmp( cmd, "G:", 2 )) {
        if (len<4) { Serial.println("Error: malformed command"); return 1; }
        green = hex_to_uint8(cmd+2)<<4;
        set_green(green);
        Serial.println("OK");        
        Serial.print("Via serial: G"); Serial.println(green);
    }

    else if (!strncmp( cmd, "B:", 2 )) {
        if (len<4) { Serial.println("Error: malformed command"); return 1; }
        blue = hex_to_uint8(cmd+2)<<4;
        set_blue(blue);
        Serial.println("OK");        
        Serial.print("Via serial: B"); Serial.println(blue);
    }
    
    else if (!strncmp( cmd, "W:", 2 )) {
        if (len<4) { Serial.println("Error: malformed command"); return 1; }
        white = hex_to_uint8(cmd+2)<<4;
        set_white(white);
        Serial.println("OK");        
        Serial.print("Via serial: W"); Serial.println(white);
    }

    else if (!strncmp( cmd, "RGBW:", 5 )) {
        if (len<13) { Serial.println("Error: malformed command");  return 1; }
        red = hex_to_uint8(cmd+5)<<4;
        green = hex_to_uint8(cmd+7)<<4;
        blue = hex_to_uint8(cmd+9)<<4;
        white = hex_to_uint8(cmd+11)<<4;
        setPWMs(red, green, blue, white);
        Serial.println("OK");        
        print_rgbw(red, green, blue, white, "Via serial: ");
    }
    
    else if (!strncmp( cmd, "OFF", 3 )) {
        setPWMs(0,0,0,0);
        Serial.println("OK");
        print_rgbw(red, green, blue, white, "Via serial: ");
    }
        
    else {
        Serial.println("Error: unknown command");
        return 1; 
    }
    
    return 0;
}


//-------------------------------------------------------------------------
// Bluetooth receive handler
// Bluetooth command format: "%04x%02x%02x%02x%02x", fadeTime, white, ref, green, blue
void handle_bluetooth() 
{
    static uint8_t index=0, err=0;
    static uint16_t  red=0, green=0, blue=0, white=0, fade=0;

    while(bluetooth.available()) {
        char c = bluetooth.read();
        // Serial.write(c); // DEBUG
        if(c == '\r' || c == '\n') { // line ending terminates command 
            if (!err && index==12) {
                setPWMs(red*16, green*16, blue*16, white*16);
                bluetooth.print('.');
                print_rgbw(red*16, green*16, blue*16, white*16, "Via bluetooth: ");
            }
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
                case 0: fade = c<<12; break;
                case 1: fade |= c<<8; break;
                case 2: fade |= c<<8; break;
                case 3: fade |= c;    break;
                case 4: white = c<<4; break;
                case 5: white |= c;   break;
                case 6: red = c<<4;   break;
                case 7: red |= c;     break;
                case 8: green = c<<4; break;
                case 9: green |= c;   break;
                case 10: blue = c<<4; break;
                case 11: blue |= c;   break;
                default: err = 1;     break;
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
    const float ir_fact = 1.05;
    if (button==0) return;
    Serial.print("Via infrared: button "); Serial.print(button); Serial.print("\n");
    switch (button) {
        case 11:  setPWMs( 0, 0, 0, 0 ); break;
        case 12:  { Savegame s = load(0); 
                    print_rgbw( s.red, s.green, s.blue, s.white, "Restore state:");
                    setPWMs(s.red, s.green, s.blue, s.white); 
                    break; }
        case 13:  { Savegame s = {get_red(), get_green(), get_blue(), get_white() };
                    print_rgbw( s.red, s.green, s.blue, s.white, "Save state:");
                    save( 0, s ); break; }
        case 21:  set_red_fact( 1./ir_fact ); break;
        case 22:  set_red_fact( ir_fact ); break; 
        case 31:  set_green_fact( 1./ir_fact ); break;
        case 32:  set_green_fact( ir_fact ); break;
        case 41:  set_blue_fact( 1./ir_fact ); break;
        case 42:  set_blue_fact( ir_fact ); break;
        case 51:  set_white_fact( 1./ir_fact ); break;
        case 52:  set_white_fact( ir_fact ); break;
    }
}

//-------------------------------------------------------------------------



