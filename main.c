/*
 * main.c
 *
 *  Created on: 15 февр. 2026 г.
 *      Author: Alex
 */

#include "main.h"


int main(){
	ht1621_init();
	lcd_clear();
	_delay_ms(1000);
	lcd_buf_clear();

	drow_input(true);
	drow_output(true);
	drow_lines_under(true);
	unsigned int counter=0;


	while(1){
		drow_digits_left(counter++);
		lcd_update();
		_delay_ms(300);
	}
}
