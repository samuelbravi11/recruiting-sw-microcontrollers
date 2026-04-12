#include "dma_usart_tx.h"
#include "main.h"
#include <stdint.h>
#include <stddef.h>

// defined on main
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_tx;

// defined on dma_usart_tx.h
uint8_t tx_buf[DIM_TX_BUF];
volatile bool tx_busy = 0;


// mutex on DMA TX
bool DMA_TX_Transfer(size_t len) {
  tx_busy = 1;
  if (HAL_UART_Transmit_DMA(&huart2, (uint8_t *)tx_buf, len) != HAL_OK) {
    tx_busy = 0;
    return false;
  }
  return true;
}

// UART callback
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    tx_busy = 0;
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  }
}