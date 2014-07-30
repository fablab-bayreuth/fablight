
#include "Store.h"

void save( uint16_t slot, uint16_t red, uint16_t green, uint16_t blue, uint16_t white )
{
    const Savegame data = {red, green, blue, white};
    save( slot, data );
}

void save( uint16_t slot, const Savegame &data )
{
    if (slot<0 || slot > SLOT_MAX) return;
    int base = slot*16;
    const uint8_t *pdata = (const uint8_t*)&data;
    for (int i=0; i<sizeof(Savegame); i++) {
        EEPROM.write( base+i, pdata[i] );
    } 
}

Savegame load( uint16_t slot )
{
    Savegame data = {0};
    int base = slot*16;
    uint8_t *pdata = (uint8_t*)&data;
    if (slot<0 || slot > SLOT_MAX) return data;
    for (int i=0; i<sizeof(Savegame); i++) {
        pdata[i] = EEPROM.read(base+i);
    } 
    return data;
}


