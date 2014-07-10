
#ifndef FABLIGHT_IR_H
#define FABLIGHT_IR_H


// Supported IR remote control types
enum fablight_ir_type {
  CARMP3   // Cheap ebay IR transmitter labelled 'Car mp3'.
};

// Call this before any other ir function
void fablight_ir_init(void);

// Get the number of the last pressed button (0 for none or error)
uint8_t fablight_ir_get_button(enum fablight_ir_type ir_type = CARMP3);


#endif // FABLIGHT_IR_H

