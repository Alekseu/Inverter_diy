/*
 * ext_adc.c
 *
 *  Created on: 19 февр. 2026 г.
 *      Author: Alex
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ext_adc.h"

#if MCP_USE_ISR==1
volatile unsigned int adc_results[CHANNELS_COUNT]; // Отфильтрованные данные
volatile spi_step_t current_step = STATE_START;
volatile unsigned char current_channel = 0;

// Обработчик прерывания завершения передачи SPI
ISR(SPI_STC_vect) {
	static unsigned char high_byte = 0;

	    switch (current_step) {
	        case STATE_START:
	            // Байт 1 отправлен, отправляем байт 2 (Конфигурация канала)
	            SPDR = (0x80 | (current_channel << 4));
	            current_step = STATE_CONF;
	            break;

	        case STATE_CONF:
	            // Байт 2 отправлен, получаем 2 старших бита, отправляем "пустышку" для Байта 3
	            high_byte = SPDR;
	            SPDR = 0x00;
	            current_step = STATE_READ;
	            break;

	        case STATE_READ:
	            // Байт 3 получен, завершаем чтение текущего канала
	        	adc_results[current_channel] = ((high_byte & 0x03) << 8) | SPDR;
	            PORTB |= (1 << PB2); // CS High

	            // --- Переход к следующему каналу ---
	            current_channel++;

	            if(current_channel>=CHANNELS_COUNT){
	            	current_step = IDLE;
	            	current_channel=0;
	            } else {
	            	//--- Автоматический запуск следующего измерения ---
	            	current_step = STATE_START;
	            	PORTB &= ~(1 << PB2); // CS Low
	            	SPDR = 0x01;          // Снова шлем стартовый байт
	            }

	            break;
	        default:
	        	current_step = IDLE;
	        	break;

	    }

}
#endif

#if MCP_USE_ISR==0
	unsigned char SPI_Transfer(unsigned char data);
#endif

void Ext_adc_init(){
    // Установка MOSI, SCK и CS на выход
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << CS_PIN);
    // MISO автоматически становится входом при включении SPI

#if MCP_USE_ISR==0
    // SPI Control Register:
    // SPE  - Включить SPI
    // MSTR - Режим Master
    // SPR0 - Предделитель частоты F_CPU/16 (для 16МГц это 1МГц, MCP3008 тянет до 3.6МГц при 5В)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    // Устанавливаем CS в HIGH (деактивация чипа)
    PORTB |= (1 << CS_PIN);
#else
    // SPCR: SPE(вкл), MSTR(мастер), SPIE(прерывание включено), SPR0(f/16)
       SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPIE) | (1 << SPR0);
       sei();
       // Запускаем первый цикл чтения
       PORTB &= ~(1 << PB2); // CS Low
       SPDR = 0x01;          // Отправляем стартовый байт
#endif

}

unsigned int MCP3008_Read(unsigned char channel) {
	if (channel > 7) return 0;

#if MCP_USE_ISR==0

    unsigned int result = 0;

    PORTB &= ~(1 << CS_PIN); // CS Low (Начало сессии)

    // Протокол MCP3008 требует 3 байта для обмена:
    // 1. Стартовый бит
    // 2. Конфигурация (SGL/DIFF, D2, D1, D0)
    // 3. Завершение чтения

    // Байт 1: Стартовый бит (0x01)
    SPI_Transfer(0x01);

    // Байт 2: Режим Single-Ended (0x80) + Номер канала (Shifted)
    // Формат: [SGL/DIFF][D2][D1][D0][X][X][X][X]
    unsigned char config = (0x80 | (channel << 4));
    unsigned char highByte = SPI_Transfer(config);

    // Байт 3: Пустой байт для получения оставшихся данных
    unsigned char lowByte = SPI_Transfer(0x00);

    PORTB |= (1 << CS_PIN); // CS High (Конец сессии)

    // MCP3008 возвращает 10 бит.
    // 2 бита в highByte (младшие) и 8 бит в lowByte.
    result = ((highByte & 0x03) << 8) | lowByte;

    return result;
#else
    return adc_results[channel];
#endif
}

#if MCP_USE_ISR==0
	unsigned char SPI_Transfer(unsigned char data) {
		SPDR = data; // Загружаем данные в регистр
		while(!(SPSR & (1 << SPIF))); // Ждем окончания передачи
		return SPDR; // Возвращаем полученный байт
	}
#endif

#if MCP_USE_ISR == 1
void StartRead(){
	  if (current_step != IDLE) return; // Если уже занято — выходим

	  current_step = STATE_START;
	  PORTB &= ~(1 << PB2); // CS Low
	  SPDR = 0x01;          // Снова шлем стартовый байт

}
#endif



