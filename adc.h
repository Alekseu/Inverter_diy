/*
 * adc.h
 *
 *  Created on: 22 февр. 2026 г.
 *      Author: Alex
 */

#ifndef ADC_H_
#define ADC_H_

// Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))

void adc_init();
unsigned int read_adc(unsigned char chanel);

#endif /* ADC_H_ */
