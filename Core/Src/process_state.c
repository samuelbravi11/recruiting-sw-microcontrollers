// this file contains the process functions for each state
#include "process_state.h"
#include "fsm.h"
#include "cli.h"
#include "guard.h"
#include "uart.h"
#include "timers.h"
#include "main.h"
#include "dma_adc.h"
#include "gpio_adc.h"
#include <string.h>

extern ADC_HandleTypeDef hadc1;

/* Init
   Initialize everything. */
void process_init(void) {
   // UART_start_receive();
   HAL_ADCEx_Calibration_Start(&hadc1);
   // DMA_ADC_Start();
   // TIM3_Base_Start(); // start timer 3 to trigger ADC

   // TODO
   // if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) {
   //    TIM1_Base_Start_IT();
   // }

   TIM1_Base_Stop_IT();
   TIM3_Base_Stop();
   TIM14_Base_Stop_IT();
   TIM16_Base_Stop_IT();
}

/* Wait Request 
   The CLI is on, the led is off, sensor reading is off */
void process_wait_request(void) {
   UART_start_receive();
   GPIO_stop_receive();
   DMA_ADC_Stop();
   TIM1_Base_Stop_IT();
   TIM3_Base_Stop();
   TIM14_Base_Stop_IT();
   TIM16_Base_Stop_IT();
}

/* Listening
   The CLI is off, the led is on and the sensor is on */
void process_listening(void) {
   UART_stop_receive();
   GPIO_start_receive();
   DMA_ADC_Start();
   TIM1_Base_Start_IT();
   TIM3_Base_Start();
   TIM14_Base_Stop_IT();
   TIM16_Base_Stop_IT();
}

/* Pause
   The CLI is on, the led is blinking with TIM14 and the sensor reading is off */
void process_pause(void) {
   UART_start_receive();
   GPIO_start_receive();
   DMA_ADC_Stop();
   TIM1_Base_Stop_IT();
   TIM3_Base_Stop();
   TIM14_Base_Start_IT();
   TIM16_Base_Stop_IT();
}

/* Warning
   The CLI is off, the led is off, sensor reading is off */
void process_warning(void) {
   UART_stop_receive();
   GPIO_stop_receive();
   DMA_ADC_Stop();
   TIM1_Base_Stop_IT();
   TIM3_Base_Stop();
   TIM14_Base_Stop_IT();
   TIM16_Base_Stop_IT();
}

/* Error
   CLI is off, sensor reading is off, the led is blinking with TIM16 */
void process_error(void) {
   UART_stop_receive();
   GPIO_stop_receive();
   DMA_ADC_Stop();
   TIM1_Base_Stop_IT();
   TIM3_Base_Stop();
   TIM14_Base_Stop_IT();
   TIM16_Base_Start_IT();
}
