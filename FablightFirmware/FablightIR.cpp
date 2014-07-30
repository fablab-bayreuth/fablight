
#include "Arduino.h"
#include "global.h"
#include "FablightIR.h"

//---------------------------------------------------------------------------------

// IMPORTANT: Use the modified IRremote library that uses timer0.
// Else PWM will not work porperly.
#include "IRremote0.h"

// Keep this private
static IRrecv0 irrecv(PIN_IR);
static decode_results results;

//---------------------------------------------------------------------------------


uint8_t fablight_ir_decode_button(unsigned long code, enum fablight_ir_type ir_type = CARMP3)
{
  static uint8_t last_button = 0;
  if (code==0)
  {
    last_button = 0;
    return 0;
  }
  switch (ir_type)
  {
    case CARMP3:
      /* Cheap ebay IR transmitter labelled 'Car mp3'.
         Button codes denote <coulmn><row>:
           11 12 13
           21 22 23
           ...
           71 72 73
         Code 0 denotes an invalid/unkown code. NEC repeat signals are translated to the
         last seen button code.
      */
      switch (code)
      {
        case 0xFFFFFFFFul: break; // repeat signal. Keep last button code.
        case 0x00FFA25Dul: last_button = 11; break;
        case 0x00FF629Dul: last_button = 12; break;
        case 0x00FFE21Dul: last_button = 13; break;
        case 0x00FF22DDul: last_button = 21; break;
        case 0x00FF02FDul: last_button = 22; break;
        case 0x00FFC23Dul: last_button = 23; break;
        case 0x00FFE01Ful: last_button = 31; break;
        case 0x00FFA857ul: last_button = 32; break;
        case 0x00FF906Ful: last_button = 33; break;
        case 0x00FF6897ul: last_button = 41; break;
        case 0x00FF9867ul: last_button = 42; break;
        case 0x00FFB04Ful: last_button = 43; break;
        case 0x00FF30CFul: last_button = 51; break;
        case 0x00FF18E7ul: last_button = 52; break;
        case 0x00FF7A85ul: last_button = 53; break;
        case 0x00FF10EFul: last_button = 61; break;
        case 0x00FF38C7ul: last_button = 62; break;
        case 0x00FF5AA5ul: last_button = 63; break;
        case 0x00FF42BDul: last_button = 71; break;
        case 0x00FF4AB5ul: last_button = 72; break;
        case 0x00FF52ADul: last_button = 73; break;
        default: last_button = 0; break;
      }
      break;
    default: last_button = 0; break;
  } // end switch fablight_ir_type

  return last_button;
}

//---------------------------------------------------------------------------------

// Call this before any other ir function.
// This initiates the IR receiver class.
void fablight_ir_init(void)
{
	irrecv.enableIRIn(); // Start the receiver
}

// Get the number of the last pressed button (0 for none or error).
// This queries the last IR code from the IR reciever class and decodes the button.
uint8_t fablight_ir_get_button(enum fablight_ir_type ir_type)
{
	uint8_t button = 0;
  	if (irrecv.decode(&results)) {
                button = fablight_ir_decode_button(results.value);
                //Serial.print("IR button=");
                //Serial.println(button);
                irrecv.resume();
	}
	return button;
}

