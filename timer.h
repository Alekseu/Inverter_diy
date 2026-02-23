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
#define WAIT_COUNT 10

void init_timers();
//timer create_timer();


typedef void Callback(void);
typedef struct {
	int time;
	unsigned int tick;
	unsigned char id;
	Callback* function;
}timers;

extern timers Timers[TIMERS_COUNT];

typedef unsigned char SetTimer(unsigned char, int,Callback*);
typedef void OffTimer(unsigned char );
typedef	void SetTimeOut(unsigned char, int );
typedef bool IsTimeOut(unsigned char);

typedef struct {
	SetTimer* settimer;
	OffTimer* offTimer;
	SetTimeOut* setTimeOut;
	IsTimeOut* isTimeOut;
}timer;

typedef bool (*IsElapsed)(void*);
typedef void (*Reset)(void*);
typedef struct {
	IsElapsed isElapsed;
	Reset reset;
	unsigned int _wait;
	unsigned int _stamp;
}wait;
void init_wait(wait* _wait, unsigned int);
#endif /* TIMER_H_ */
