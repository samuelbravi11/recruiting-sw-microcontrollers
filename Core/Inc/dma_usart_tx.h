#ifndef DMA_USART_H
#define DMA_USART_H

#include "dma_adc.h"
#include <stdbool.h>
#include <stddef.h>

// process half of adc_buf at time
#define DIM_TX_BUF 8 * (DIM_ADC_BUF / 2)  // max 8 chars for "val_adc + \t + val_gpio + \r\n"

extern uint8_t tx_buf[DIM_TX_BUF];
extern volatile bool tx_busy;

bool DMA_TX_transfer(size_t len);
bool DMA_TX_transfer_buf(uint8_t *buf, size_t len);

#endif