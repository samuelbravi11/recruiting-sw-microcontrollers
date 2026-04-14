#ifndef DMA_CONTROLLER_H
#define DMA_CONTROLLER_H

#include "fsm_types.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// FSM_RET process_first_block(void);
// FSM_RET process_second_block(void);
void check_and_process(void);
void set_process_callback(void);

#endif // DMA_CONTROLLER_H