#ifndef UART_H
#define UART_H


#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


void uart_init(uint32_t baud);

uint16_t uart_rx(uint8_t *buf, uint16_t size);
uint16_t uart_rx_avail(void);

uint16_t uart_tx(const uint8_t *buf, uint16_t size);
uint16_t uart_tx_free(void);


#ifdef __cplusplus
}
#endif


#endif
