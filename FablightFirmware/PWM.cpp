#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "PWM.h"

static uint16_t timer1[2] = {0,0};
void setTimer1PWM() {
  /* Timer 1: Pins 9 and 10.
   * If values are small (<10), use CTC mode and a 12 bit timer.
   * If values are <20, use a 10-bit fast PWM.
   * Else, use an 8-bit fast PWM.
   */
  
  TCCR1A = 0;
  
  if(timer1[0] > 0) {
    TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
    OCR1A = timer1[0];
  } else {
    PORTB |= (1<<1);
  }
  
  if(timer1[1] > 0) {
    TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
    OCR1B = timer1[1];
  } else {
    PORTB |= (1<<2);
  }
  
  if((timer1[0] > 0 && timer1[0] < 10) || (timer1[1] > 0 && timer1[1] < 10)) {
    TCCR1A |= (1<<WGM11); 
    TCCR1B = (1<<WGM13) | (1<<CS10);
    ICR1 = 0x0FFF;
  } else if((timer1[0] > 0 && timer1[0] < 20) || (timer1[1] > 0 && timer1[1] < 20)) {
    TCCR1A = (1<<WGM10) | (1<<WGM11); 
    TCCR1B = (1<<WGM12) | (1<<CS10);
    OCR1A >>= 2;
    OCR1B >>= 2;
  } else {
    TCCR1A |= (1<<WGM10); 
    TCCR1B = (1<<WGM12) | (1<<CS10);
    OCR1A >>= 4;
    OCR1B >>= 4;
  }
}

static uint16_t timer2[2] = {0,0};
static uint8_t timer2isrctr = 0, timer2isrmax = 0;
static uint8_t timer2cmpa = 0, timer2cmpb = 0;
void setTimer2PWM() {
  /* Timer 2: Pins 3 and 11
   * If values are <20, the overflow ISR uses an additional counter (2 or 4 bits) to switch between two OCR values
   */
   
  // Disable the overflow interrupt first
  TIMSK2 &= ~(1<<TOIE2);
  
  // phase-correct PWM
  TCCR2A = (1<<WGM21) | (1<<WGM20);
  
  if(timer2[0] > 0) {
    // Inverting mode
    TCCR2A |= (1<<COM2A1) | (1<<COM2A0);
    OCR2A = (timer2[0] >> 4)&0xFF + ((timer2[0] & 0xF) > 0 ? 1 : 0);
  } else {
    PORTB |= (1<<3);
  }
  
  if(timer2[1] > 0) {
    // Inverting mode
    TCCR2A |= (1<<COM2B1) | (1<<COM2B0);
    OCR2B = (timer2[1] >> 4) + ((timer2[1] & 0xF) > 0 ? 1 : 0);
  } else {
    PORTD |= (1<<3);
  }
  
  if(((timer2[0]&0xF) > 0 && timer2[0] < 10) || ((timer2[1]&0xF) > 0 && timer2[1] < 10)) {
    /* 4-bit counter */
    timer2isrctr = 0;
    timer2isrmax = 0xF;
    timer2cmpa = timer2[0] & 0xF;
    timer2cmpb = timer2[1] & 0xF;
    TIMSK2 |= (1<<TOIE2);
  } else if(((timer2[0]&0xC) > 0 && timer2[0] < 20) || ((timer2[1]&0xC) > 0 && timer2[1] < 20)) {
    /* 2-bit counter */
    timer2isrctr = 0;
    timer2isrmax = 0x3;
    timer2cmpa = (timer2[0]>>2) & 0x3;
    timer2cmpb = (timer2[1]>>2) & 0x3;
    TIMSK2 |= (1<<TOIE2);
  }
}

void setPWM(uint8_t color, uint16_t value) {
  uint8_t pin = colorToPin[color];
  switch(pin) {
  case 3:
    if(value > 0x0FF0) value &= ~0xF;
    timer2[0] = value;
    setTimer2PWM();
    break;
  case 9:
    timer1[0] = value;
    setTimer1PWM();
    break;
  case 10:
    timer1[1] = value;
    setTimer1PWM();
    break;
  case 11:
    if(value > 0x0FF0) value &= ~0xF;
    timer2[1] = value;
    setTimer2PWM();
    break;
  }
}

void initPWM() {
  pinMode(PIN_W, OUTPUT);
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  
  TCCR2B = (1<<CS20);
  
  setTimer1PWM();
  setTimer2PWM();
}

ISR(TIMER2_OVF_vect) {
  OCR2A = (timer2[0] >> 4) + (timer2isrctr < timer2cmpa);
  OCR2B = (timer2[1] >> 4) + (timer2isrctr < timer2cmpb);
  
  timer2isrctr = (timer2isrctr+1) & timer2isrmax;
}
