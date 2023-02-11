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

volatile int edge = 0;

volatile int period = 0;

char String[25];

void Initialize() {
	
	cli();
	
	//Pin 13 - LED_BUILTIN
	DDRB |= (1<<DDB5); //output
	
	//Pin 8
	DDRB &= ~(1<<DDB0); //input
	
	//Clock Setup
	TCCR1B &= ~(1<<CS10);
	TCCR1B |= (1<<CS11);
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
	
	sei();
}


ISR(TIMER1_CAPT_vect) {
	edge = ICR1;
	
	if (PINB & (1<<PINB0)) {
		//HIGH
		PORTB |= (1<<PORTB5);
	} else {
		//LOW
		PORTB &= ~(1<<PORTB5);
	}
	
	//Toggle
	TCCR1B ^= (1<<ICES1);
}

int main(void) {
	Initialize();
	UART_init(BAUD_PRESCALER);
    while (1) {	
		sprintf(String,"%u\n", edge);
		UART_putstring(String);
		_delay_ms(1000);
    }
}

