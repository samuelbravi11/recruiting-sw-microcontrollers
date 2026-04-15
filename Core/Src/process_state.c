// this file contains the process functions for each state
#include "process_state.h"
#include "cli.h"
#include "dma_adc.h"
#include "exti.h"
#include "fsm.h"
#include "gpio_adc.h"
#include "main.h"
#include "timers.h"
#include "uart.h"
#include <string.h>

extern ADC_HandleTypeDef hadc1;

/* Init
   Initialize everything. */
FSM_RET process_init(void) {
   // UART_start_receive();
   if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
      set_error_message("ADC calibration failed");
      return FSM_ERROR;
   }
   if (TIM1_Base_Stop_IT() != FSM_OK || TIM3_Base_Stop() != FSM_OK ||
       TIM14_Base_Stop_IT() != FSM_OK || TIM16_Base_Stop_IT() != FSM_OK) {
      return FSM_ERROR;
   }
   return FSM_OK;
}

/* Wait Request
   The CLI is on, the led is off, sensor reading is off */
FSM_RET process_wait_request(void) {
   if (UART_start_receive() != FSM_OK) {
      return FSM_ERROR;
   }
   GPIO_stop_receive();
   if (DMA_ADC_Stop() != FSM_OK) {
      return FSM_ERROR;
   }
   if (TIM1_Base_Stop_IT() != FSM_OK || TIM3_Base_Stop() != FSM_OK ||
       TIM14_Base_Stop_IT() != FSM_OK || TIM16_Base_Stop_IT() != FSM_OK) {
      return FSM_ERROR;
   }
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
   return FSM_OK;
}

/* Listening
   The CLI is off, the led is on and the sensor is on */
FSM_RET process_listening(void) {
   if (UART_stop_receive() != FSM_OK) {
      return FSM_ERROR;
   }
   GPIO_start_receive();
   frsHalfCplt = false;
   scdHalfCplt = false;
   if (DMA_ADC_Start() != FSM_OK)
      return FSM_ERROR;
   if (TIM3_Base_Start() != FSM_OK || TIM14_Base_Stop_IT() != FSM_OK ||
       TIM16_Base_Stop_IT() != FSM_OK) {
      return FSM_ERROR;
   }
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
   if (HAL_GPIO_ReadPin(GPIO_PORT_ADC, GPIO_PIN_ADC) == GPIO_PIN_SET) {
      gpio_led_adc = 1;
      if (TIM1_Base_Start_IT() != FSM_OK) {
         return FSM_ERROR;
      }
   } else {
      gpio_led_adc = 0;
      if (TIM1_Base_Stop_IT() != FSM_OK) {
         return FSM_ERROR;
      }
   }
   return FSM_OK;
}

/* Pause
   The CLI is on, the led is blinking with TIM14 and the sensor reading is off
 */
FSM_RET process_pause(void) {
   if (UART_start_receive() != FSM_OK) {
      return FSM_ERROR;
   }
   GPIO_stop_receive();
   if (DMA_ADC_Stop() != FSM_OK) {
      return FSM_ERROR;
   }
   if (TIM1_Base_Stop_IT() != FSM_OK || TIM3_Base_Stop() != FSM_OK ||
       TIM14_Base_Start_IT() != FSM_OK || TIM16_Base_Stop_IT() != FSM_OK) {
      return FSM_ERROR;
   }
   return FSM_OK;
}

/* Warning
   The CLI is off, the led is off, sensor reading is off */
FSM_RET process_warning(void) {
   if (UART_stop_receive() != FSM_OK) {
      return FSM_ERROR;
   }
   GPIO_stop_receive();
   if (DMA_ADC_Stop() != FSM_OK) {
      return FSM_ERROR;
   }
   if (TIM1_Base_Stop_IT() != FSM_OK || TIM3_Base_Stop() != FSM_OK ||
       TIM14_Base_Stop_IT() != FSM_OK || TIM16_Base_Stop_IT() != FSM_OK) {
      return FSM_ERROR;
   }
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
   return FSM_OK;
}

/* Error
   CLI is off, sensor reading is off, the led is blinking with TIM16 */
FSM_RET process_error(void) {
   if (UART_stop_receive() != FSM_OK) {
      return FSM_ERROR;
   }
   GPIO_stop_receive();
   if (DMA_ADC_Stop() != FSM_OK) {
      return FSM_ERROR;
   }
   if (TIM1_Base_Stop_IT() != FSM_OK || TIM3_Base_Stop() != FSM_OK ||
       TIM14_Base_Stop_IT() != FSM_OK || TIM16_Base_Start_IT() != FSM_OK) {
      return FSM_ERROR;
   }
   return FSM_OK;
}
