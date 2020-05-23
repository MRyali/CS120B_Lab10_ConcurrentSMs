/*	Author: lab
 *  Partner(s) Name: Mayur Ryali
 *	Lab Section: 21
 *	Assignment: Lab #10  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "io.h"
#endif

enum States_1 {Start_1, light1, light2, light3} state1; //ThreeLEDsSM
enum States_2 {Start_2, on, off} state2; //BlinkingLEDSM
enum States_3 {Start_3, on1, off1} state3; //SpeakerSM
enum States_4 {Start_4, wait, inc, incRelease, dec, decRelease} state4; //FrequencySM
//enum States_3 {Start_3, combine} state3; //CombineLEDsSM

unsigned char threeLEDs; //temp for sequential lighting value
unsigned char blinkingLED; //temp for blinking lighting value
unsigned char count = 0;
unsigned char switchA2; //PA2

unsigned char incButton; //PA0
unsigned char decButton; //PA1

const double notes[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25}; //array of frequencies
unsigned char i = 0; //index for array

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
			state = Start_1;
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
/*
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
*/
void SpeakerSM() {
    switch(state3) {
        case Start_3:
            state3 = off1; //start powered off
            break;
        case off1:
            count = 0;
            if (switchA2) { //if switch is on
                state3 = on1;
            }
            else { //otherwise stat off
                state3 = off1;
            }
            break;
        case ON3:
            if (count < 2 && switchA2) { //stay on for 2ms
                state3 = ON3;
            }
            else {
                count++; //increment the count
                state3 = off1;
            }
            break;
        default:
            state3 = Start_3;
            break;
    }
    switch(state3) {
        case Start_3:
            break;
        case off1:
            PORTB = 0x00 | blinkingLED | threeLEDs; //turn off PB4 while keeping LED functionality
            break;
        case on1:
            PORTB = notes[i] | blinkingLED | threeLEDs;  //turn on PB4 while keeping LED functionality
            break;
        }
        default:
            break;
}

void FrequencySM() {
    switch(state4) {
        case Start_4:
            state4 = wait;
            break;
        case wait:
            if (incButton) {
                state4 = inc;
            }
            else if (decButton) {
                state4 = dec;
            }
            else {
                state4 = wait;
            }
            break;
        case inc:
            state4 = incRelease;
            break;
        case incRelease:
            if (!incButton) {
                state4 = wait;
            }
            else {
                state4 = incRelease;
            }
            break;
        case dec:
            state4 = decRelease;
            break;
        case decRelease:
            if (!decButton) {
                state4 = wait;
            }
            else {
                state4 = decRelease;
            }
            break;
        default:
            state4 = Start_4;
            break;
    }
    switch(state4) {
        case Start_4:
            break;
        case wait:
            break;
        case inc:
            if (i < 7) {
                i++;
            }
            break;
        case incRelease:
            break;
        case dec:
            if (i > 0) {
                i--;
            }
            break;
        case decRelease:
            break;
        default:
            break;
    }
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF; //input
    DDRB = 0xFF; PORTB = 0x00; //output

    unsigned long ThreeLED_Timer = 300; //300ms
    unsigned long BlinkLED_Timer = 1000; //1000ms
    unsigned long Speaker_Timer = 2; //2ms
    unsigned long period = 1; //1ms => GCD of the other three periods

    TimerSet(period); //set timer here
    TimerOn();

    state1 = Start_1; //start state
    state2 = Start_2;
    state3 = Start_3;
    state4 = start_4;

    while (1) {
        switchA2 = ~PINA & 0x02; //button on PA2
        incButton = ~PINA & 0x00; //button on PA0
        decButton = ~PINA & 0x01; //button on PA!
        if (ThreeLED_Timer >= 300) { //every 300ms run ThreeLED
            ThreeLEDsSM();
            ThreeLED_Timer = 0; //reset
        }
        if (BlinkLED_Timer >= 1000) { //every 1000ms run BlinkLED
            BlinkingLEDSM();
            BlinkLED_Timer = 0; //reset
        }
        if (BlinkLED_Timer >= 2) { //every 1000ms run BlinkLED
            SpeakerSM();
            Speaker_Timer = 0; //reset
        }

        FrequencySM();

        while (!TimerFlag) {}
        TimerFlag = 0;

        ThreeLED_Timer += period; //increase timer by the period
        BlinkLED_Timer += period; //increase timer by the period
        Speaker_Timer += period; //increae timer by the period
    }
    return 1;
}
