#ifndef DMA_ADC_H
#define DMA_ADC_H

#define DIM_ADC_BUF 256

#include "fsm_types.h"
#include <stdbool.h>
#include <stdint.h>

extern volatile bool frsHalfCplt;
extern volatile bool scdHalfCplt;
extern uint16_t adc_buf[DIM_ADC_BUF];

FSM_RET DMA_ADC_Start(void);
FSM_RET DMA_ADC_Stop(void);

#endif