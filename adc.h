/*
 * adc.h
 *
 *  Created on: 22 февр. 2026 г.
 *      Author: Alex
 */

#ifndef ADC_H_
#define ADC_H_

#define USE_ISR 1
// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))

#define FIRST_ADC_INPUT 0
#define LAST_ADC_INPUT 7

void adc_init();

unsigned int read_adc(unsigned char chanel);

#if USE_ISR == 1
	void StartRead_internal();
#endif

#endif /* ADC_H_ */
