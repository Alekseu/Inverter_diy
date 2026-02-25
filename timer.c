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
volatile timer Timers[TIMERS_COUNT];
unsigned char timer_counter=0;

unsigned char getTimeById(char id);

ISR(TIMER0_OVF_vect)
{
	// Reinitialize Timer 0 value
	TCNT0=0x44;

	_SystemTick++;

	for(unsigned char i = 0; i < timer_counter; i++){
		if (Timers[i].tick > 0){
			Timers[i].tick--;
			continue;
		}

		if(Timers[i].function != 0){
			Timers[i].tick = Timers[i].time;
			Timers[i].function();
		}
	}
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

unsigned char setTimer(unsigned char id, int time, CallBack f){
	if (id <= 0)
		return 0;

	Timers[timer_counter].id =id;
	Timers[timer_counter].time =time;
	Timers[timer_counter].tick = time;
	Timers[timer_counter].function = f;

	return timer_counter++;
}

void killTimer(uint8_t id){
	char nTimer = getTimeById(id);
	if (nTimer <= 0)
			return;

	timer_counter--;
}

void setTimeOut(uint8_t id, int time){
	unsigned char nTimer = getTimeById(id);

	if (nTimer <= 0)
		return;

	if (time == -1)
		Timers[nTimer].tick = Timers[nTimer].time;
	else{
		Timers[nTimer].tick = time;
		Timers[nTimer].time = time;
	}
}

bool isTimeOut(uint8_t id){
	unsigned char nTimer = getTimeById(id);

	if (nTimer <= 0)
		return false;

	if (Timers[nTimer].tick)
		return true;
	else
		return false;
}

unsigned char getTimeById(char id){
	for (unsigned char i = 0; i < timer_counter; i++)
			if (Timers[i].id == id)
				return i;

		return -1;
}

//wait functions
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


