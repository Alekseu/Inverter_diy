/*
 * main.c
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */

#include "main.h"

unsigned char lcd_update_counter=0;

int main(){

	inverter_init();

	while(1){
		if(lcd_update_counter++>=10){
			lcd_update_counter=0;
			lcd_update();
		}
		inverter_process();
		_delay_ms(10);
	}
}

