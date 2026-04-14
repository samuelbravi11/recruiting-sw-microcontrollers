#ifndef USART_H
#define USART_H

#include "main.h"
#include "fsm_types.h"

void UART_start_receive(void);
void UART_stop_receive(void);
bool UART_is_running(void);
FSM_RET UART_transmit(const char *tx_msg, uint8_t Size);
char *UART_get_user_input(void);
FSM_RET UART_poll_errors(void);

#endif