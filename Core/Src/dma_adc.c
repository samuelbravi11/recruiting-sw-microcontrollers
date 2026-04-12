#include "dma_adc.h"
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// defined on main
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

// defined on dma_adc.h
volatile bool frsHalfCplt = 0;
volatile bool scdHalfCplt = 0;
uint16_t adc_buf[DIM_ADC_BUF];


// DMA start reading from ADC
void DMA_ADC_Start(void) {
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buf, DIM_ADC_BUF);
}

// ADC callback
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1) {
    // flag per far scrivere alla cpu su prima metà
    frsHalfCplt = 1;
  }
}

// ADC callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1) {
    // flag per far scrivere alla cpu su seconda metà
    scdHalfCplt = 1;
  }
}