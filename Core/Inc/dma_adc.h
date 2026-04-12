#ifndef DMA_ADC_H
#define DMA_ADC_H

#define DIM_ADC_BUF 16

#include <stdint.h>
#include <stdbool.h>

extern volatile bool frsHalfCplt;
extern volatile bool scdHalfCplt;
extern uint16_t adc_buf[DIM_ADC_BUF];

void DMA_ADC_Start(void);

#endif