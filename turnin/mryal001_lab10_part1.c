/*	Author:  Mayur Ryali
 *  Partner(s) Name:
 *	Lab Section: 21
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

 // Demo: https://drive.google.com/open?id=129Deopj3dI-W9vgHtMZXoMMFAj-TYmtM

#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "io.h"
#endif

enum States_1 {Start_1, light1, light2, light3} state1; //ThreeLEDsSM
enum States_2 {Start_2, on, off} state2; //BlinkingLEDSM
enum States_3 {Start_3, combine} state3; //CombineLEDsSM

unsigned char threeLEDs; //temp for sequential lighting value
unsigned char blinkingLED; //temp for blinking lighting value

void ThreeLEDsSM() {
    switch(state1) { //sequential LED lighting
	    case Start_1:
		    state1 = light1;
            break;
        case light1:
			state1 = light2;
			break;
		case light2:
			state1 = light3;
			break;
		case light3:
			state1 = light1;
			break;
		default:
			state1 = Start_1;
			break;
	}
	switch (state1) {
		case Start_1:
			break;
		case light1:
			threeLEDs = 0x01; //PB0 on
			break;
		case light2:
			threeLEDs = 0x02; //PB1 on
			break;
		case light3:
			threeLEDs = 0x04; //PB2 on
			break;
		default:
			break;
	}
}

void BlinkingLEDSM() { //blink PB3
	switch(state2) {
		case Start_2:
			state2 = on;
			break;
		case on:
			state2 = off;
			break;
		case off:
			state2 = on;
			break;
		default:
			state2 = Start_2;
			break;
	}
	switch (state2) {
		case Start_2:
			blinkingLED = 0x00; //PB3 starts off
			break;
		case on:
			blinkingLED = 0x08; //PB3 on
			break;
		case off:
			blinkingLED = 0x00; //PB3 off
			break;
		default:
			break;
	}
}

void CombineLEDsSM() { //combine LED values into PORTB
	switch(state3) {
		case Start_3:
			state3 = combine;
			break;
		case combine:
			state3 = combine;
			break;
		default:
			state3 = Start_3;
			break;
	}
	switch(state3) {
		case Start_3:
			break;
		case combine:
			PORTB = blinkingLED | threeLEDs;
			break;
		default:
			break;
	}
}

int main(void) {
    DDRB = 0xFF; PORTB = 0x00; //output

    TimerSet(1000); //set timer here (1s)
    TimerOn(); //turn on timer

    state1 = Start_1; //start state
    state2 = Start_2;

    while (1) {
      ThreeLEDsSM();
      BlinkingLEDSM();
	  CombineLEDsSM();

      while (!TimerFlag) {}
      TimerFlag = 0;
    }
    return 1;
}
