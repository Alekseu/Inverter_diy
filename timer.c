/*
 * timer.c
 *
 *  Created on: 22 февр. 2026 г.
 *      Author: Alex
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

volatile unsigned long _SystemTick = 0;
timers Timers[TIMERS_COUNT];

ISR(TIMER0_OVF_vect)
{
	// Reinitialize Timer 0 value
	TCNT0=0x44;
	// Place your code here
	_SystemTick++;
}


void init_timers(){
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 187,500 kHz
	// Mode: Normal top=0xFF
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	// Timer Period: 1,0027 ms
	TCCR0A=(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
	TCCR0B=(0<<WGM02) | (0<<CS02) | (1<<CS01) | (1<<CS00);
	TCNT0=0x44;
	OCR0A=0x00;
	OCR0B=0x00;

	// Timer/Counter 0 Interrupt(s) initialization
	TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);
	sei();
}



bool _IsElapsed(void* _wait){
	if (((wait*)_wait)->_stamp > _SystemTick) return true;

	if (((wait*)_wait)->_stamp + ((wait*)_wait)->_wait < _SystemTick) return true;

	return false;
}

void _Reset(void* _wait){
	((wait*)_wait)->_stamp = _SystemTick;
}

void init_wait(wait* _wait_ptr, unsigned int _wait){
	_wait_ptr->_wait = _wait;
	_wait_ptr->_stamp = _SystemTick;
	_wait_ptr->isElapsed = &_IsElapsed;
	_wait_ptr->reset = &_Reset;
}


