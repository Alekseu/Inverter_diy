/*
 * adc.c
 *
 *  Created on: 22 февр. 2026 г.
 *      Author: Alex
 */

#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>
#include "adc.h"

#if USE_ISR == 1
   volatile unsigned int adc_data[LAST_ADC_INPUT-FIRST_ADC_INPUT+1];
   volatile unsigned char input_index=0;
   volatile unsigned char idle =1;
#endif

#if USE_ISR ==1
ISR(ADC_vect){
	// Read the AD conversion result
	adc_data[input_index]=ADCW;
	// Select next ADC input
	if (++input_index > (LAST_ADC_INPUT-FIRST_ADC_INPUT)){
		input_index=0;
		idle =1;
	}

	ADMUX=(FIRST_ADC_INPUT | ADC_VREF_TYPE)+input_index;

	//_delay_us(10);

	if(idle==0){
		// Start the AD conversion
		ADCSRA|=(1<<ADSC);
	}

}
#endif

void adc_init(){
#if USE_ISR == 0
	//инициализаци ацп без прерываний
	// ADC initialization
	// ADC Clock frequency: 93,750 kHz
	// ADC Voltage Reference: AVCC pin
	// ADC Auto Trigger Source: Free Running
	// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
	// ADC4: Off, ADC5: Off
	DIDR0=(1<<ADC5D) | (1<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (0<<ADC0D);
	ADMUX=ADC_VREF_TYPE;
	ADCSRA=(1<<ADEN) | (0<<ADSC) | (1<<ADATE) | (0<<ADIF) | (0<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
#else
//инициализаци ацп с прерываними
	// ADC initialization
	// ADC Clock frequency: 93,750 kHz
	// ADC Voltage Reference: AREF pin
	// ADC Auto Trigger Source: ADC Stopped
	// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
	// ADC4: Off, ADC5: Off
	DIDR0=(1<<ADC5D) | (1<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (0<<ADC0D);
	ADMUX=FIRST_ADC_INPUT | ADC_VREF_TYPE;
	ADCSRA=(1<<ADEN) | (1<<ADSC) | (0<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRB=(0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);
#endif

}

//чтение ацп
unsigned int read_adc(unsigned char chanel){
#if USE_ISR ==0
	ADMUX=chanel | ADC_VREF_TYPE;
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	// Start the AD conversion
	ADCSRA|=(1<<ADSC);
	// Wait for the AD conversion to complete
	while ((ADCSRA & (1<<ADIF))==0);
	ADCSRA|=(1<<ADIF);
	return ADCW;
#else
	if(chanel>(LAST_ADC_INPUT-FIRST_ADC_INPUT))
		return 0;

	return adc_data[chanel];
#endif
}

#if USE_ISR == 1
	void StartRead_internal(){
		if(idle!=1) return;

		idle=0;
		// Start the AD conversion
		ADCSRA|=(1<<ADSC);
	}
#endif
