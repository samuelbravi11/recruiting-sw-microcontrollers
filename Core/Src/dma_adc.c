#include "dma_adc.h"
#include "fsm.h"
#include "fsm_types.h"
#include "main.h"
#include <stdbool.h>
#include <stdint.h>


// defined on main
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

// defined on dma_adc.h
volatile bool frsHalfCplt = 0;
volatile bool scdHalfCplt = 0;
uint16_t adc_buf[DIM_ADC_BUF];

// start ADC generating DMA requests
FSM_RET DMA_ADC_Start(void) {
   FSM_RET status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buf, DIM_ADC_BUF);
   if (status != FSM_OK) {
      set_error_message("DMA ADC start failed");
   }
   return status;
}

// stop ADC generating DMA requests
FSM_RET DMA_ADC_Stop(void) {
   FSM_RET status = HAL_ADC_Stop_DMA(&hadc1);
   if (status != FSM_OK) {
      set_error_message("DMA ADC stop failed");
   }
   return status;
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