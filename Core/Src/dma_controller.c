#include "dma_controller.h"
#include "cli.h"
#include "dma_adc.h"
#include "dma_usart_tx.h"
#include "exti.h"
#include "fsm.h"
#include "queue_event.h"
#include <stdio.h>
#include <string.h>


#define MA_SIZE 150

static void (*adc_process_callback)(uint16_t *val_adc);
// moving average buffer and variables
static uint32_t ma_buf[MA_SIZE] = {0};
static uint32_t ma_sum = 0;
static uint16_t ma_idx = 0;
static uint16_t ma_count = 0;

// here i can apply some processing on adc_buf before transfering to tx_buf, for
// example, i can apply a low-pass filter to the adc data, or i can convert the
// adc data to voltage values before sending them out via USART.
static FSM_RET process_block(size_t offset, size_t count) {
   size_t w_pos = 0;
   for (size_t i = 0; i < count; i++) {
      uint16_t val_adc = adc_buf[offset + i];
      // callback function to process adc value before sending it
      if (adc_process_callback) {
         adc_process_callback(&val_adc);
      }
      // snprintf return the size of the string formatted: '67\t1\r\n' = 6
      int n = snprintf((char *)&tx_buf[w_pos], DIM_TX_BUF - w_pos, "%u\t%d\r\n",
                       val_adc, gpio_led_adc);
      if (n < 0) {
         break;
      }
      if ((size_t)n >= (DIM_TX_BUF - w_pos)) { // not enough space in tx_buf
         break;
      }
      w_pos += (size_t)n;
   }
   if (w_pos > 0 && !DMA_TX_transfer(w_pos)) {
      set_error_message("DMA transfer to TX UART failed");
      return FSM_ERROR;
   }
   return FSM_OK;
}

static FSM_RET process_first_block(void) {
   return process_block(0, DIM_ADC_BUF / 2);
}

static FSM_RET process_second_block(void) {
   return process_block(DIM_ADC_BUF / 2, DIM_ADC_BUF / 2);
}

// if tx is busy i can skip the current block and wait for the next one,
// this is not a good solution because i can lose data, a better
// solution but more complex is to use multiple buffers were the DMA can
// write while the tx is busy.
FSM_RET check_and_process(void) {
   if (frsHalfCplt) {
      frsHalfCplt = 0;
      if (!tx_busy && process_first_block() == FSM_ERROR) {
         if (!enqueue_event(EVENT_INVALID))
            return FSM_ERROR;
      }
   }
   if (scdHalfCplt) {
      scdHalfCplt = 0;
      if (!tx_busy && process_second_block() == FSM_ERROR) {
         if (!enqueue_event(EVENT_INVALID))
            return FSM_ERROR;
      }
   }
   return FSM_OK;
}

static void reset_filters(void) {
   memset(ma_buf, 0, sizeof(ma_buf));
   ma_sum = 0;
   ma_idx = 0;
   ma_count = 0;
}

void process_raw(uint16_t *val) { /* nothing to do */ }

void process_moving_average(uint16_t *val) {
   ma_sum -= ma_buf[ma_idx];
   ma_buf[ma_idx] = *val;
   ma_sum += *val;
   ma_idx = (ma_idx + 1) % MA_SIZE;
   if (ma_count < MA_SIZE)
      ma_count++;
   *val = (uint16_t)(ma_sum / ma_count);
}

void process_random_noise(uint16_t *val) {
   static uint16_t lfsr = 0xACE1u;
   lfsr ^= lfsr >> 7;
   lfsr ^= lfsr << 9;
   lfsr ^= lfsr >> 13;
   int32_t noisy =
       (int32_t)(*val) + ((int32_t)(lfsr & 0xFF) - 128); // rumore ±128 LSB
   if (noisy < 0)
      noisy = 0;
   if (noisy > 4095)
      noisy = 4095;
   *val = (uint16_t)noisy;
}

void set_process_callback(void) {
   reset_filters();
   switch (cli_last_cmd) {
   case CMD_RAW:
      adc_process_callback = process_raw;
      break;
   case CMD_MOVING_AVERAGE:
      adc_process_callback = process_moving_average;
      break;
   case CMD_RANDOM_NOISE:
      adc_process_callback = process_random_noise;
      break;
   default:
      adc_process_callback = 0;
      break;
   }
}