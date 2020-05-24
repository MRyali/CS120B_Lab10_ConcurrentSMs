/*	Author: test
 *  Partner(s) Name:
 *	Lab Section: 021
 *	Assignment: Lab #10  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;


void TimerOn () {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff () {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet (unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

#define button (~PINA & 0x04)

enum states1 {start1, led1, led2, led3} state1;
enum states2 {start2, on1, off1} state2;
enum states3 {start3, combine} state3;
enum states4 {start4, off2, on2} state4;

unsigned char threeLEDS;
unsigned char blinkingLED;
unsigned char buzzer;
unsigned char i;
unsigned short j;

void ThreeLEDsSM() {
	switch (state1) {
		case start1:
			state1 = led1;
			i = 0;
			break;
		case led1:
			if (i == 150) {
				state1 = led2;
				i = 0;
			}
			else {
				state1 = led1;
			}
			break;
		case led2:
			if (i == 150) {
				state1 = led3;
				i = 0;
			}
			else {
				state1 = led2;
			}
			break;
		case led3:
			if (i == 150) {
				state1 = led1;
				i = 0;
			}
			else {
				state1 = led3;
			}
			break;
	}
	switch (state1) {
		case start1:
			break;
		case led1:
			threeLEDS = 0x01;
			i++;
			break;
		case led2:
			threeLEDS = 0x02;
			i++;
			break;
		case led3:
			threeLEDS = 0x04;
			i++;
		break;
	}
}

void BlinkingLEDSM() {
	switch (state2) {
		case start2:
			state2 = on1;
			break;
		case on1:
			if (j == 500) {
				state2 = off1;
				j = 0;
			}
			else {
				state2 = on1;
			}
			break;
		case off1:
			if (j == 500) {
				state2 = on1;
				j = 0;
			}
			else {
				state2 = off1;
			}
			break;
	}
	switch (state2) {
		case start2:
			blinkingLED = 0x00;
			break;
		case on1:
			blinkingLED = 0x08;
			j++;
			break;
		case off1:
			blinkingLED = 0x00;
			j++;
			break;
	}
}

void Speaker() {
	switch (state4) {
		case start4:
			state4 = off2;
			break;
		case off2:
			if (button) {
				state4 = on2;
			}
			else {
				state4 = off2;
			}
			break;
		case on2:
			state4 = off2;
			break;
	}
	switch (state4) {
		case start4:
			break;
		case off2:
			buzzer = 0x00;
			break;
		case on2:
			buzzer = 0x10;
			break;
	}
}

void CombineLEDsSM() {
	switch (state3) {
		case start3:
			state3 = combine;
			break;
		case combine:
			state3 = combine;
			break;
	}
	switch (state3) {
		case start3:
			break;
		case combine:
			PORTB = threeLEDS | blinkingLED | buzzer;
			break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(2);
	TimerOn();
	state1 = start1;
	state2 = start2;
	state3 = start3;
	while (1) {
		ThreeLEDsSM();
		BlinkingLEDSM();
		Speaker();
		CombineLEDsSM();
		while (!TimerFlag);
		TimerFlag = 0;
	}
}
