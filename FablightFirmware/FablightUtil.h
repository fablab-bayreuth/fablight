
inline void print_rgbw( uint16_t red, uint16_t green, uint16_t blue, uint16_t white, const char* msg = 0 )
{
    if (msg) {
        Serial.print(msg);
        Serial.print(' ');
    }
    Serial.print('R'); Serial.print(red); 
    Serial.print(" G"); Serial.print(green);
    Serial.print(" B"); Serial.print(blue);
    Serial.print(" W"); Serial.print(white);
    Serial.print('\n');
}
