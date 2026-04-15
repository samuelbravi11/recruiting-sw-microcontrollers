#include "cli.h"
#include "fsm.h"
#include "fsm_types.h"
#include "dma_usart_tx.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#define DIM_BUF_READ 20
#define DIM_BUF_ERR 64

extern UART_HandleTypeDef huart2;

static volatile uint8_t byte_read;
static volatile uint8_t idx = 0;
static volatile uint8_t line_read = 0;
static volatile uint8_t flag_error = 0;

static char rx_buff[DIM_BUF_READ];
static char line_copy[DIM_BUF_READ];

static char msg_err[DIM_BUF_ERR];

FSM_RET UART_start_receive(void) {
   FSM_RET status = HAL_UART_Receive_IT(&huart2, (uint8_t *)&byte_read, 1);
   if (status != FSM_OK) {
      set_error_message("UART start receive failed");
   }
   return status;
}

FSM_RET UART_stop_receive(void) {
   FSM_RET status = HAL_UART_AbortReceive_IT(&huart2);
   if (status != FSM_OK) {
      set_error_message("UART stop receive failed");
   }
   return status;
}

bool UART_is_running(void) {
   HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart2);
   return (state == HAL_UART_STATE_BUSY_RX) || (state == HAL_UART_STATE_BUSY_TX_RX);
}

// every error leads to an error state --> to reset UART press the user button
FSM_RET UART_poll_errors(void) {
   switch (flag_error)
   {
   case 0:
      return FSM_OK;
   case 1:
      strcpy(msg_err, "Wrong huart");
      break;
   case 2:
      strcpy(msg_err, "Full buffer");
      break;
   case 3:
      strcpy(msg_err, "UART ErrorCallback");
      break;
   case 4:
      strcpy(msg_err, "UART RxCallback, RX arrived before previous line was consumed");
      break;
   default:
      strcpy(msg_err, "Unknown error");
      break;
   }
   flag_error = 0;
   idx = 0;
   line_read = 0;
   // clean buffer
   memset(rx_buff, 0, sizeof(rx_buff));
   memset(line_copy, 0, sizeof(line_copy));
   set_error_message(msg_err);
   return FSM_ERROR;
}

// polling if line is read --> in case print \r\n, copy to buffer and start UART
char *UART_get_user_input(void) {
   if (!line_read)
      return 0;
   DMA_TX_transfer_buf((uint8_t *)"\r\n", 2);
   __disable_irq();
   strcpy(line_copy, rx_buff);
   idx = 0;
   line_read = 0;
   __enable_irq();
   UART_start_receive();
   return line_copy;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
   if (huart->Instance != USART2) {
      flag_error = 1;
      return;
   }
   // ignore the input while cpu not processed line read
   if (line_read) {
      flag_error = 4;
      UART_start_receive();
      return;
   }
   // echo ch with DMA
   DMA_TX_transfer_buf((uint8_t *)&byte_read, 1);
   // behavior like other byte but don't copy in rx_buf
   if (byte_read == '\n') {
      UART_start_receive();
      return;
   }
   // stop receive byte --> handle after process line
   if (byte_read == '\r') {
      rx_buff[idx] = 0;
      line_read = 1;
      return;
   }  
   if (idx < DIM_BUF_READ - 1) {
      rx_buff[idx++] = (char)byte_read;
      UART_start_receive();
   } else {
      idx = 0;
      rx_buff[0] = 0;
      flag_error = 2;
      UART_start_receive();
   }
}

// callback in case of error, we clear the flags and restart the reception
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
   if (huart->Instance == USART2) {
      __HAL_UART_CLEAR_OREFLAG(huart);
      __HAL_UART_CLEAR_FEFLAG(huart);
      __HAL_UART_CLEAR_NEFLAG(huart);
      idx = 0;
      line_read = 0;
      flag_error = 3;
      UART_start_receive();
   }
}