#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "PWM.h"
//---------------------------------------------------------------------

uint8_t colorToPin[4] = {
    PIN_W, PIN_R, PIN_G, PIN_B };

// Private functions
void applyTimer1PWM(void);
void applyTimer2PWM(void);

// Private data
static uint16_t   // pwm duty cycles 
    timer1A = 0,  // blue
    timer1B = 0,  // green
    timer2A = 0,  // red
    timer2B = 0;  // white  
    
static struct {           // modulator states for timer 2
    uint8_t duty12;  // duty cycles bits <12:4>
    uint8_t duty4;   // duty cacles bits <3:0>
    uint8_t lag;     // mismatch accumulator
}
timer2A_state = {
    0,0,0}
,
timer2B_state = {
    0,0,0};

//---------------------------------------------------------------------

void initPWM() {
    pinMode(PIN_W, OUTPUT);
    pinMode(PIN_R, OUTPUT);
    pinMode(PIN_G, OUTPUT);
    pinMode(PIN_B, OUTPUT);

    // The timers work in dual slope mode, counts 0 -> TOP -> 0.
    // The PWM pin is active while TCNT1 > OCR1x. Overflow interrupt is triggered
    // if TCNT1 reaches 0.

    // Timer 1 (16bit): No prescaler, Mode 10 (Phase correct PWM)
    TCCR1A = (1<<WGM11) ;
    TCCR1B = (1<<WGM13) | (1<<CS10);
    ICR1 = 0x0FFF;           // Timer 1 TOP value --> 12 bit resolution
    // Frequency = Fosc/0xfff/2 = 1.954 kHz

    // Timer 2 (8bit): No prescaler, Mode 1 (phase correct PWM), TOP=0xFF
    TCCR2A = (1<<WGM21) | (1<<WGM20);
    TCCR2B = (1<<CS20);
    // Frequency = Fosc/0xff/2 = 31.373 kHz

    applyTimer1PWM();
    applyTimer2PWM();
}

//---------------------------------------------------------------------

void applyTimer1PWM(void) {
    // Timer 1: Pins 9 (blue) and 10 (green)
    // 12 bit PWM
    // The new duty cycles are not applied immediately, but in the next timer 1 overflow isr.

    TIMSK1 &= ~(1<<TOIE1);  // Disable timer 1 overflow interrupt

    // Timer 1 channel A -> pin 9 (blue)
    if (timer1A > 0xFFF) timer1A = 0xFFF;
    if (timer1A > 0) {
        TCCR1A |= (1<<COM1A1) | (1<<COM1A0);  // PWM pin 9 active low
    }
    else {
        TCCR1A &= ~(1<<COM1A1) & ~(1<<COM1A0); // Pin 9 disconnected from PWM
        PORTB |= (1<<1);                       // Set high manually
    }

    // Timer 1 channel B -> pin 10 (green)
    if (timer1B > 0xFFF) timer1B = 0xFFF;
    if (timer1B > 0) {
        TCCR1A |= (1<<COM1B1) | (1<<COM1B0);  // PWM pin 10 active low
    }
    else {
        TCCR1A &= ~(1<<COM1B1) & ~(1<<COM1B0); // Pin 10 disconnected from PWM
        PORTB |= (1<<2);                       // Set high manually
    }

    TIMSK1 |= (1<<TOIE1);  // Reenable timer 1 overflow interrupt
    // NOTE: Will be disabled in the timer 1 overflow isr after setting the new duty cycles

}

//---------------------------------------------------------------------

void applyTimer2PWM(void) {
    // Timer 2: Pins 3 (white) and 11 (red)
    // 8 bit native PWM. For duty cycles <64, an additional 4 bit Sigma-Delta-Modulator
    // will be used to get 12 bit total resolution (implemented in the timer 2 overflow isr)
    // The new duty claces will not be applied immediately, but in the next timer 2 overflow isr.

    TIMSK2 &= ~(1<<TOIE2);  // Disable timer 2 overflow interrupt

    // Timer 2 channel A -> pin 11 (red)
    if (timer2A > 0xFF0) timer2A = 0xFF0;
    if (timer2A > 0) {
        TCCR2A |= (1<<COM2A1) | (1<<COM2A0);  // PWM pin 11 active low
    }
    else {
        TCCR2A &= ~(1<<COM2A1) & ~(1<<COM2A0);  // Pin 11 disconnected from PWM
        PORTB |= (1<<3);                        // Set high manually
    }

    // Timer 2 channel B -> pin 3 (white)
    if (timer2B > 0xFF0) timer2B = 0xFF0;
    if (timer2B > 0) {
        TCCR2B |= (1<<COM2B1) | (1<<COM2B0);  // PWM pin 3 active low
    }
    else {
        TCCR2B &= ~(1<<COM2B1) & ~(1<<COM2B0);  // Pin 3 disconnected from PWM
        PORTD |= (1<<3);                       // Set high manually
    }

    // SD-modulator state
    timer2A_state.duty12 = (timer2A >> 4);
    timer2A_state.duty4  = (timer2A     ) & 0x0F;
    //timer2A_state.lag    = 0; // keep accumulator running
    timer2B_state.duty12 = (timer2B >> 4);
    timer2B_state.duty4  = (timer2B     ) & 0x0F;
    //timer2B_state.lag    = 0; // keep accumulator running

    // Enable timer 2 overflow interrupt (SD-modulator)
    TIMSK2 |= (1<<TOIE2);
}

//---------------------------------------------------------------------

void setPWM(uint8_t color, uint16_t value) {
    uint8_t pin = colorToPin[color];
    switch(pin) {
    case 3: // white
        timer2B = value;
        applyTimer2PWM();
        break;
    case 9: // blue
        timer1A = value;
        applyTimer1PWM();
        break;
    case 10: // green
        timer1B = value;
        applyTimer1PWM();
        break;
    case 11: // red
        timer2A = value;
        applyTimer2PWM();
        break;
    }
}

//---------------------------------------------------------------------

void setPWM_rel(uint8_t color, int16_t diff) {
    uint8_t pin = colorToPin[color];
    int16_t t = 0;
    switch(pin) {
    case 3: // white
        t = timer2B + diff;
        if (t<0) t=0;
        if (t>0xfff) t=0xfff;
        timer2B = t;
        applyTimer2PWM();
        break;
    case 9: // blue
        t = timer1A + diff;
        if (t<0) t=0;
        if (t>0xfff) t=0xfff;
        timer1A = t;
        applyTimer1PWM();
        break;
    case 10: // green
        t = timer1B + diff;
        if (t<0) t=0;
        if (t>0xfff) t=0xfff;
        timer1A = t;
        applyTimer1PWM();
        break;
    case 11: // red
        t = timer2A + diff;
        if (t<0) t=0;
        if (t>0xfff) t=0xfff;
        timer2A = t;
        applyTimer2PWM();
        break;
    }
}

//---------------------------------------------------------------------

void setPWMs(uint16_t white, uint16_t red, uint16_t green, uint16_t blue)
{
    timer1A = blue;
    timer1B = green;
    timer2A = red;
    timer2B = white;
    applyTimer1PWM();
    applyTimer2PWM();
}

//---------------------------------------------------------------------

void setPWMs_rel(int16_t white, int16_t red, int16_t green, int16_t blue)
{
    int16_t t;

    t = timer1A + blue;
    if (t<0) t=0;
    if (t>0xfff) t=0xfff;
    timer1A = t;

    t = timer1B + green;
    if (t<0) t=0;
    if (t>0xfff) t=0xfff;
    timer1B = t;

    t = timer2A + red;
    if (t<0) t=0;
    if (t>0xfff) t=0xfff;
    timer2A = t;

    t = timer2B + white;
    if (t<0) t=0;
    if (t>0xfff) t=0xfff;
    timer2B = t;

    applyTimer1PWM();
    applyTimer2PWM();
}

//---------------------------------------------------------------------
// Single color setters
void set_red(uint16_t value)
{
    timer2A = value;
    applyTimer2PWM();
}

void set_green(uint16_t value)
{
    timer1B = value;
    applyTimer1PWM();
}

void set_blue(uint16_t value)
{
    timer1A = value;
    applyTimer1PWM();
}

void set_white(uint16_t value)
{
    timer2B = value;
    applyTimer2PWM();
}

//---------------------------------------------------------------------
// Single color getters

uint16_t get_red(void) { return timer2A; }
uint16_t get_green(void) { return timer1B; }
uint16_t get_blue(void) { return timer1A; }
uint16_t get_white(void) { return timer2B; }

//---------------------------------------------------------------------
// Relative (additive) setters 

void set_red_rel(int16_t diff)
{
    int16_t t = timer2A + diff;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer2A = t;
    applyTimer2PWM();
}

void set_green_rel(int16_t diff)
{
    int16_t t = timer1B + diff;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer1B = t;
    applyTimer1PWM();
}

void set_blue_rel(int16_t diff)
{
    int16_t t = timer1A + diff;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer1A = t;
    applyTimer1PWM();
}

void set_white_rel(int16_t diff)
{
    int16_t t = timer2B + diff;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer2B = t;
    applyTimer2PWM();
}

//---------------------------------------------------------------------
// Relative (by factor) setters
// Minimum change is +/- 1

void set_red_fact(float fact)
{
    int16_t  t = timer2A * fact;
    if (t == timer2A)
        if (fact > 1.) t++;
        else if (fact < 1.) t--;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer2A = t;
    applyTimer2PWM();
}

void set_green_fact(float fact)
{
    int16_t  t = timer1B * fact;
    if (t == timer1B)
        if (fact > 1.) t++;
        else if (fact < 1.) t--;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer1B = t;
    applyTimer1PWM();
}

void set_blue_fact(float fact)
{
    int16_t t = timer1A * fact;
    if (t == timer1A)
        if (fact > 1.) t++;
        else if (fact < 1.) t--;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer1A = t;
    applyTimer1PWM();
}

void set_white_fact(float fact)
{
    int16_t t = timer2B * fact;
    if (t == timer2B)
        if (fact > 1.) t++;
        else if (fact < 1.) t--;
    if (t < 0) t = 0;
    if (t > 0xfff) t = 0xfff;
    timer2B = t;
    applyTimer2PWM();
}

//---------------------------------------------------------------------
// Timer 2 overflow interrupt service routine
//   This implements a simple 4 bit sigma-delta modulator for the Timer 2 PWMs to increase the resolution to 12 bit.
//   The 4 bit mismatch is accumulated in timer2x_state.lag, bit 4 overflow of this is added to
//   the next pwm duty cycle value. Therefore, the actual duty cycle is switched between 2 values,
//   giving a 12 bit resolution on average.

ISR(TIMER2_OVF_vect) {
    register uint8_t reg;

    timer2A_state.lag += timer2A_state.duty4;   // Accumulate
    reg = timer2A_state.duty12;     // Keep duty cycle in register
    if(timer2A_state.lag & 0x10) {  // Add phase accumulator bit 4 overflow
        reg++;
    }
    timer2A_state.lag &= 0x0F;

    if(reg == 0) {
        TCCR2A &= ~(3<<COM2A0);  // PWM output off
    }
    else {
        OCR2A = reg;          // Apply duty cycle
        TCCR2A |= 3<<COM2A0;  // PWM output on
    }

    // Same for B channel
    timer2B_state.lag += timer2B_state.duty4;
    reg = timer2B_state.duty12;
    if(timer2B_state.lag & 0x10) {
        reg++;
    }
    timer2B_state.lag &= 0x0F;

    if(reg == 0) {
        TCCR2A &= ~(3<<COM2B0);
    }
    else {
        OCR2B = reg;
        TCCR2A |= 3<<COM2B0;
    }
}

//---------------------------------------------------------------------
// Timer 1 overflow interrupt service routine
// Load compare registers with new PWM duty values.

ISR(TIMER1_OVF_vect) {
    OCR1A = timer1A;
    OCR1B = timer1B;
    TIMSK1 &= ~(1<<TOIE1); // Disable timer 1 overflow interrupt ( will be re-enabled by applysTimer1PWM() ).
}



