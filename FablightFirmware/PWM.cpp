#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "PWM.h"

static uint16_t timer1[2] = {1,1};
void setTimer1PWM() {
	/* Timer 1: Pins 9 and 10.
	 * Uses CTC mode and a 12 bit timer.
	 */
  
	TCCR1A = 0;
  
	if(timer1[0] > 0) {
		TCCR1A |= (1<<COM1A1) | (1<<COM1A0);
	} else {
		PORTB |= (1<<1);
	}
  
	if(timer1[1] > 0) {
		TCCR1A |= (1<<COM1B1) | (1<<COM1B0);
	} else {
		PORTB |= (1<<2);
	}
  
	TCCR1A |= (1<<WGM11); 
	TIMSK1 |= (1<<TOIE1);
}
static uint16_t timer2[2] = {1,1};
static struct {
	uint8_t duty12[2];
	uint8_t duty4[2];
	uint8_t lag[2];
} timer2_state;
static uint8_t timer2ctr = 0;
static uint8_t timer2cmpa = 0, timer2cmpb = 0;
void setTimer2PWM() {
	/* Timer 2: Pins 3 and 11
	 * If one of the PWM values is <64, the overflow ISR will use an
	 * additional counter (4 bits) to switch between two OCR values
	 */
   
	// Disable the overflow interrupt first
	TIMSK2 &= ~(1<<TOIE2);
  
	// phase-correct PWM
	TCCR2A = (1<<WGM21) | (1<<WGM20);
  
	if(timer2[0] > 0) {
		// Inverting mode
		TCCR2A |= (1<<COM2A1) | (1<<COM2A0);
		OCR2A = (timer2[0] >> 4);
	} else {
		PORTB |= (1<<3);
	}
  
	if(timer2[1] > 0) {
		// Inverting mode
		TCCR2A |= (1<<COM2B1) | (1<<COM2B0);
		OCR2B = (timer2[1] >> 4);
	} else {
		PORTD |= (1<<3);
	}
  
	if((timer2[0] > 0 && timer2[0] < 64) || (timer2[1] > 0 && timer2[1] < 64)) {
		timer2_state.duty12[0] = (timer2[0] >> 4);
		timer2_state.duty4 [0] = (timer2[0]     ) & 0x0F;
		timer2_state.lag   [0] = 0;
		timer2_state.duty12[1] = (timer2[1] >> 4);
		timer2_state.duty4 [1] = (timer2[1]     ) & 0x0F;
		timer2_state.lag   [1] = 0;
		TIMSK2 |= (1<<TOIE2);
	}
}

void setPWM(uint8_t color, uint16_t value) {
	uint8_t pin = colorToPin[color];
	switch(pin) {
	case 3:
		if(value > 0x0FF0) value &= ~0xF;
		timer2[1] = value;
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
		timer2[0] = value;
		setTimer2PWM();
		break;
	}
}

void initPWM() {
	pinMode(PIN_W, OUTPUT);
	pinMode(PIN_R, OUTPUT);
	pinMode(PIN_G, OUTPUT);
	pinMode(PIN_B, OUTPUT);

	TCCR1B = (1<<WGM13) | (1<<CS10);
	ICR1 = 0x0FFF;

	TCCR2B = (1<<CS20);
  
	setTimer1PWM();
	setTimer2PWM();

	DDRD |= 0x80;
	PORTD |= 0x80;
	PORTD &= ~0x7F;
}

ISR(TIMER2_OVF_vect) {
	register uint8_t reg;
	timer2_state.lag[0] += timer2_state.duty4[0];
	reg = timer2_state.duty12[0];
	if(timer2_state.lag[0] & 0x10) {
		reg++;
	}

	if(reg == 0) {
		TCCR2A &= ~(3<<COM2A0);
	} else {
		OCR2A = reg;
		TCCR2A |= 3<<COM2A0;
	}
	timer2_state.lag[0] &= 0x0F;

	timer2_state.lag[1] += timer2_state.duty4[1];
	reg = timer2_state.duty12[1];
	if(timer2_state.lag[1] & 0x10) {
		reg++;
	}

	if(reg == 0) {
		TCCR2A &= ~(3<<COM2B0);
	} else {
		OCR2B = reg;
		TCCR2A |= 3<<COM2B0;
	}
	timer2_state.lag[1] &= 0x0F;
}

ISR(TIMER1_OVF_vect) {
	OCR1A = timer1[0];
	OCR1B = timer1[1];
	TIMSK1 &= ~(1<<TOIE1);
}
