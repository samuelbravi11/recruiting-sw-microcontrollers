#include "dma_controller.h"
#include "dma_adc.h"
#include "dma_usart_tx.h"
#include <stdio.h>

// here i can apply some processing on adc_buf before transfering to tx_buf, for example, i can apply a low-pass filter to the adc data, or i can convert the adc data to voltage values before sending them out via USART.
static void process_block(size_t offset, size_t count) {
  size_t w_pos = 0;
  for (size_t i = 0; i < count; i++) {
    uint16_t val_adc = adc_buf[offset + i];
    // snprintf return the size of the string formatted: '67\r\n' = 4
    int n = snprintf((char *)&tx_buf[w_pos], DIM_TX_BUF - w_pos, "%u\r\n", val_adc);
    if (n < 0) {
      break;
    }
    if ((size_t)n >= (DIM_TX_BUF - w_pos)) {  // not enough space in tx_buf
      break;
    }
    w_pos += (size_t)n;
  }
  if (w_pos > 0) {
      DMA_TX_Transfer(w_pos);
  }
}

void process_first_block(void) {
  process_block(0, DIM_ADC_BUF / 2);
}

void process_second_block(void) {
  process_block(DIM_ADC_BUF / 2, DIM_ADC_BUF / 2);
}

// if tx is busy i can skip the current block and wait for the next one, 
// this is not a good solution because i can lose data, a better 
// solution but more complex is to use multiple buffers were the DMA can 
// write while the tx is busy.
void check_and_process(void) {
  if (frsHalfCplt) {
    frsHalfCplt = 0;
    if (!tx_busy) {
      process_first_block();
    }
  }
  if (scdHalfCplt) {
    scdHalfCplt = 0;
    if (!tx_busy) {
      process_second_block();
    }
  }
}