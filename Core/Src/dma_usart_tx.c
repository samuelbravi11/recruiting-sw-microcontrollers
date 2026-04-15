#include "dma_usart_tx.h"
#include "fsm.h"
#include "main.h"
#include <stddef.h>
#include <stdint.h>


// defined on main
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_tx;

// defined on dma_usart_tx.h
uint8_t tx_buf[DIM_TX_BUF];
volatile bool tx_busy = 0;

// busy wait on DMA TX
bool DMA_TX_transfer(size_t len) {
   while (tx_busy)
      ; // wait DMA UART enter TxCallback --> TODO: timeout
   tx_busy = 1;
   if (HAL_UART_Transmit_DMA(&huart2, (uint8_t *)tx_buf, len) != HAL_OK) {
      // set_error_message("DMA UART TX transfer failed");
      tx_busy = 0;
      return false;
   }
   return true;
}

bool DMA_TX_transfer_buf(uint8_t *buf, size_t len) {
   while (tx_busy)
      ; // wait DMA UART enter TxCallback --> TODO: timeout
   tx_busy = 1;
   if (HAL_UART_Transmit_DMA(&huart2, buf, len) != HAL_OK) {
      // set_error_message("DMA UART TX transfer failed");
      tx_busy = 0;
      return false;
   }
   return true;
}

// DMA UART callback
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
   if (huart->Instance == USART2) {
      tx_busy = 0;
   }
}