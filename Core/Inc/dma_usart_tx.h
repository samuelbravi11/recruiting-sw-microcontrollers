#ifndef DMA_USART_H
#define DMA_USART_H

#include "dma_adc.h"
#include <stdbool.h>
#include <stddef.h>

// process half of adc_buf at time
#define DIM_TX_BUF 6 * (DIM_ADC_BUF / 2)  // max 6 chars for "val_adc + \r\n"

extern uint8_t tx_buf[DIM_TX_BUF];
extern volatile bool tx_busy;

bool DMA_TX_Transfer(size_t len);
void USART_Transmit(uint8_t *data, size_t len);

#endif