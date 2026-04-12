#ifndef DMA_CONTROLLER_H
#define DMA_CONTROLLER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void process_first_block(void);
void process_second_block(void);
void check_and_process(void);

#endif // DMA_CONTROLLER_H