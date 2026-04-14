#include "cli.h"
#include "fsm_types.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

#define DIM_BUFFER_READ 20

extern UART_HandleTypeDef huart2;

static volatile uint8_t byte_read;
static volatile uint8_t idx = 0;
static volatile uint8_t line_read = 0;
static volatile uint8_t flag_error = 0;

static char rx_buff[DIM_BUFFER_READ];
static char line_copy[DIM_BUFFER_READ];

void UART_start_receive(void) {
   HAL_UART_Receive_IT(&huart2, (uint8_t *)&byte_read, 1);
}

void UART_stop_receive(void) {
   HAL_UART_AbortReceive_IT(&huart2);
}

bool UART_is_running(void) {
   return HAL_UART_GetState(&huart2) == HAL_UART_STATE_BUSY_RX;
}

FSM_RET UART_transmit(const char *tx_msg, uint8_t Size) {
   return HAL_UART_Transmit(&huart2, (uint8_t *)tx_msg, Size, 100);
}

FSM_RET UART_poll_errors(void) {
   if (flag_error) {
      flag_error = 0;
      idx = 0;
      line_read = 0;
      for (int i = 0; i < DIM_BUFFER_READ; i++) {
         rx_buff[i] = 0;
      }
      uint8_t msg_err[] = "Huart sbagliata o buffer troppo piccolo\r\n";
      UART_transmit(msg_err, strlen(msg_err));
      // UART_start_receive();
      return FSM_ERROR;
   }
   return FSM_OK;
}

char *UART_get_user_input(void) {
   if (!line_read) {
      return NULL;
   }
   rx_buff[idx] = '\0';
   strcpy(line_copy, rx_buff);
   line_read = 0;
   idx = 0;
   UART_start_receive();
   return line_copy;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
   if (huart->Instance != USART2) {
      flag_error = 1;
      return;
   }
   if (byte_read == '\r') {
      line_read = 1;
      return;
   }
   if (byte_read == '\n') {
      return;
   }
   if (idx < DIM_BUFFER_READ - 1) {
      rx_buff[idx] = (char)byte_read;
      idx++;
      HAL_UART_Receive_IT(&huart2, (uint8_t *)&byte_read, 1);
   } else {
      flag_error = 1;
   }
}