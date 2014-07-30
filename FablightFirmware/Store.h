
// Save/restore device state.  
// 16 bytes per slot: RGBW state (8byte), 8 unused bytes. The Arduino Uno has 
// 1024 bytes of Eeeprom memory, so there are 64 possible slots.


#ifndef STORE_H
#define STORE_H

#include <EEPROM.h>
#include <stdint.h>

#define SLOT_MAX (1024/16-1)

#pragma pack(1)
struct Savegame {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t white;
};

void save( uint16_t slot, uint16_t red, uint16_t green, uint16_t blue, uint16_t white );
void save( uint16_t slot, const Savegame &data );
Savegame load( uint16_t slot );

#endif // STORE_H
