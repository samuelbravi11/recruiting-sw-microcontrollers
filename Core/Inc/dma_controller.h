#ifndef DMA_CONTROLLER_H
#define DMA_CONTROLLER_H

#include "fsm_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

FSM_RET check_and_process(void);
void set_process_callback(void);

#endif // DMA_CONTROLLER_H