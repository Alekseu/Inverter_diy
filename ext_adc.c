/*
 * ext_adc.c
 *
 *  Created on: 19 февр. 2026 г.
 *      Author: Alex
 */

#include "ext_adc.h"

spi_state_t current_state = IDLE;
volatile uint8_t adc_channel = 0;
volatile uint16_t adc_result = 0;
volatile uint8_t ready_flag = 0; // 1, когда данные готовы

volatile uint16_t adc_buffer[8] = {0};
volatile uint8_t buffer_index = 0;
volatile uint16_t filtered_value = 0;

unsigned char SPI_Transfer(unsigned char data);

void SPI_Init(){
    // Установка MOSI, SCK и CS на выход
    DDRB |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << CS_PIN);
    // MISO автоматически становится входом при включении SPI

    // SPI Control Register:
    // SPE  - Включить SPI
    // MSTR - Режим Master
    // SPR0 - Предделитель частоты F_CPU/16 (для 16МГц это 1МГц, MCP3008 тянет до 3.6МГц при 5В)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    // Устанавливаем CS в HIGH (деактивация чипа)
    PORTB |= (1 << CS_PIN);

}

void SPI_Init_Interrupt() {
    // Пины: PB2(CS), PB3(MOSI), PB5(SCK) на выход
    DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5);
    PORTB |= (1 << PB2); // CS High

    // SPCR: SPE(вкл), MSTR(мастер), SPIE(прерывание включено), SPR0(f/16)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPIE) | (1 << SPR0);

    sei(); // Глобальное разрешение прерываний
}

// Функция запуска измерения (неблокирующая)
void MCP3008_StartRead(unsigned char channel) {
    if (current_state != IDLE) return; // Если уже занято — выходим

    adc_channel = channel;
    ready_flag = 0;
    current_state = SEND_START;

    PORTB &= ~(1 << PB2);    // CS Low
    SPDR = 0x01;             // Отправляем стартовый байт (активирует прерывание по завершению)
}

unsigned int MCP3008_Read(unsigned char channel) {
    if (channel > 7) return 0;

    uint16_t result = 0;

    PORTB &= ~(1 << CS_PIN); // CS Low (Начало сессии)

    // Протокол MCP3008 требует 3 байта для обмена:
    // 1. Стартовый бит
    // 2. Конфигурация (SGL/DIFF, D2, D1, D0)
    // 3. Завершение чтения

    // Байт 1: Стартовый бит (0x01)
    SPI_Transfer(0x01);

    // Байт 2: Режим Single-Ended (0x80) + Номер канала (Shifted)
    // Формат: [SGL/DIFF][D2][D1][D0][X][X][X][X]
    uint8_t config = (0x80 | (channel << 4));
    uint8_t highByte = SPI_Transfer(config);

    // Байт 3: Пустой байт для получения оставшихся данных
    uint8_t lowByte = SPI_Transfer(0x00);

    PORTB |= (1 << CS_PIN); // CS High (Конец сессии)

    // MCP3008 возвращает 10 бит.
    // 2 бита в highByte (младшие) и 8 бит в lowByte.
    result = ((highByte & 0x03) << 8) | lowByte;

    return result;
}

unsigned char SPI_Transfer(unsigned char data) {
    SPDR = data; // Загружаем данные в регистр
    while(!(SPSR & (1 << SPIF))); // Ждем окончания передачи
    return SPDR; // Возвращаем полученный байт
}


// Обработчик прерывания завершения передачи SPI
ISR(SPI_STC_vect) {
    static uint8_t highByte = 0;

    switch (current_state) {
        case SEND_START:
            current_state = SEND_CONF;
            // Отправляем байт конфигурации (SGL + Channel)
            SPDR = (0x80 | (adc_channel << 4));
            break;

        case SEND_CONF:
            current_state = READ_DATA;
            highByte = SPDR; // Сохраняем пришедший байт (там 2 бита результата)
            SPDR = 0x00;     // Отправляем «пустышку» для получения хвоста данных
            break;

        case READ_DATA:
            // Собираем 10-битный результат
            adc_result = ((highByte & 0x03) << 8) | SPDR;
            PORTB |= (1 << PB2); // CS High (конец сессии)

            current_state = IDLE;
            ready_flag = 1;      // Сигнал основному циклу
            break;

        default:
            current_state = IDLE;
            break;
    }
}


