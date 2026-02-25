/*
 * ext_adc.h
 *
 *  Created on: 19 февр. 2026 г.
 *      Author: Alex
 */

#ifndef EXT_ADC_H_
#define EXT_ADC_H_

#define MCP_USE_ISR 1

// Определения пинов (Порт B)
#define CS_PIN    PB2  // Chip Select (SS)
#define MOSI_PIN  PB3
#define MISO_PIN  PB4
#define SCK_PIN   PB5

#if MCP_USE_ISR == 1
	#define CHANNELS_COUNT 8

	// Глобальные переменные (volatile, так как меняются в прерывании)
	extern volatile unsigned int adc_results[CHANNELS_COUNT]; // Отфильтрованные данные
	// Состояния для конечного автомата прерывания
	typedef enum {
		IDLE,       // Покой
		STATE_START,
		STATE_CONF,
		STATE_READ
	} spi_step_t;

#endif

void Ext_adc_init();

unsigned int MCP3008_Read(unsigned char channel);

#if MCP_USE_ISR == 1
	void StartRead();
#endif

#endif /* EXT_ADC_H_ */
