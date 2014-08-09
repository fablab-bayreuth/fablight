
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


// Convert a 2-digit hexadecimal string an 8-bit integer
// Invalid digits are returned as 0, there is no error indicator
inline uint8_t  hex_to_uint8( const char* hex )
{
    uint8_t n=0;
    char c = hex[0];
    if (c >= '0' && c <= '9')  n += (c-'0')<<4; 
    else if (c >= 'A' && c <= 'F')  n += (c-'A'+10)<<4;
    else return 0;
    c = hex[1];
    if (c >= '0' && c <= '9')  n += (c-'0'); 
    else if (c >= 'A' && c <= 'F')  n += (c-'A'+10);
    else return 0;
    return n;
}
