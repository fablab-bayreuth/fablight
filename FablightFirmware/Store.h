
#ifndef STORE_H
#define STORE_H

#include <EEPROM.h>
#include <stdint.h>

#define STORE_MAX (1024/16-1)

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
