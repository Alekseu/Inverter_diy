/*
 * ext_adc.h
 *
 *  Created on: 19 февр. 2026 г.
 *      Author: Alex
 */

#ifndef EXT_ADC_H_
#define EXT_ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>

// Определения пинов (Порт B)
#define CS_PIN    PB2  // Chip Select (SS)
#define MOSI_PIN  PB3
#define MISO_PIN  PB4
#define SCK_PIN   PB5

#define AVG_WINDOW 8  // Размер окна усреднения

// Состояния обмена
typedef enum {
    IDLE,       // Покой
    SEND_START, // Отправка 0x01
    SEND_CONF,  // Отправка конфигурации канала
    READ_DATA   // Чтение последнего байта
} spi_state_t;

extern spi_state_t current_state;
extern volatile uint16_t adc_result;
extern volatile uint8_t ready_flag;

void SPI_Init();
void SPI_Init_Interrupt();
void MCP3008_StartRead(unsigned char channel);
unsigned int MCP3008_Read(unsigned char channel);

#endif /* EXT_ADC_H_ */
