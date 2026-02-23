/*
 * timer.h
 *
 *  Created on: 22 февр. 2026 г.
 *      Author: Alex
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "types.h"

#define TIMERS_COUNT 5

typedef void CallBack(void);
typedef struct {
	int time;
	unsigned int tick;
	unsigned char id;
	CallBack* function; //коллбэк будет работать из прерывания (не применть на тяжолые функции)
}timer;

extern volatile timer Timers[TIMERS_COUNT];

typedef bool (*IsElapsed)(void*);
typedef void (*Reset)(void*);
typedef struct {
	IsElapsed isElapsed;
	Reset reset;
	unsigned int _wait;
	unsigned int _stamp;
}wait;

void init_wait(wait* _wait, unsigned int);
void init_timers();

unsigned char setTimer(unsigned char id, int time, CallBack f);
void killTimer(uint8_t id);
void setTimeOut(uint8_t id, int time);
bool isTimeOut(uint8_t id);

#endif /* TIMER_H_ */
