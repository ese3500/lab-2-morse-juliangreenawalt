/*
 * ESE-350-Lab2.c
 *
 * Created: 1/27/2023 2:27:51 PM
 * Author : Julian
 */ 

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

/*--------------------Libraries---------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"

volatile int overflowCount = 0;

volatile int on = 0;

char letter[4] = "XXXX";

int index = 0;

char String[25];

void Initialize() {
	
	cli();
	
	DDRB |= (1<<DDB2); //output
	DDRB |= (1<<DDB3); //output
	DDRB |= (1<<DDB4); //output
	
	
	//Pin 8
	DDRB &= ~(1<<DDB0); //input
	
	//Clock Setup
	TCCR1B |= (1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B &= ~(1<<CS12);
	
	//Set Timer
	TCCR1A &= ~(1<<WGM10);
	TCCR1A &= ~(1<<WGM11);
	TCCR1B &= ~(1<<WGM12);
	TCCR1B &= ~(1<<WGM13);
	
	//Look for rising edge
	TCCR1B |= (1<<ICES1);
	
	//Clear interrupt flag
	TIFR1 |= (1<<ICF1);
	
	//Enable input capture interrupt
	TIMSK1 |= (1<<ICIE1);
	
	TIMSK1 |= (1<<TOIE1);
	
	sei();
}

ISR (TIMER1_OVF_vect)
{
	++overflowCount;
}

ISR(TIMER1_CAPT_vect) {
	
	//Clear interrupt flag
	TIFR1 |= (1<<ICF1);
	if (on == 1) {
		on = 0;
		if (overflowCount > 5 && overflowCount < 90) {
			//DOT
			PORTB |= (1<<PORTB2);
			PORTB &= ~(1<<PORTB3);
			PORTB &= ~(1<<PORTB4);
			letter[index] = 'd';
			++index;
		}
		if (overflowCount >= 90) {
			//DASH
			PORTB &= ~(1<<PORTB2);
			PORTB |= (1<<PORTB3);
			PORTB &= ~(1<<PORTB4);
			letter[index] = 'D';
			++index;
		}
	} else {
		on = 1;
		if (overflowCount > 300) {
			//SPACE
			PORTB &= ~(1<<PORTB2);
			PORTB &= ~(1<<PORTB3);
			PORTB |= (1<<PORTB4);
			index = 0;
			printLetter(letter);
			letter[0] = 'X';
			letter[1] = 'X';
			letter[2] = 'X';
			letter[3] = 'X';
		}
	}
	
	overflowCount = 0;
	
	//Toggle
	TCCR1B ^= (1<<ICES1);
}

void printLetter(char let[4]) {

	if (let[0] == 'd') {
		if (let[1] == 'D') {
			if (let[2] == 'd') {
				if (let[3] == 'd') {
					sprintf(String, "L");
					UART_putstring(String);
				}
			}
			if (let[2] == 'D') {
				if (let[3] == 'D') {
					sprintf(String,"J");
					UART_putstring(String);
				}
			}
		}
		if (let[1] == 'd') {
			if (let[2] == 'D') {
				sprintf(String,"U");
				UART_putstring(String);
			}
		}
	}
}

int main(void) {
	Initialize();
	UART_init(BAUD_PRESCALER);
    while (1) {	
		
    }
}

