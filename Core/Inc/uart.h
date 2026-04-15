#ifndef USART_H
#define USART_H

#include "fsm_types.h"
#include "main.h"

FSM_RET UART_start_receive(void);
FSM_RET UART_stop_receive(void);
bool UART_is_running(void);
char *UART_get_user_input(void);
FSM_RET UART_poll_errors(void);

#endif