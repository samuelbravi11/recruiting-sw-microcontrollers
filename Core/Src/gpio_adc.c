#include "gpio_adc.h"
#include "timers.h"
#include <stdint.h>

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_ADC) {
    // START TIMER 1
    TIM1_Base_Start_IT();
  }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_ADC) {
    // STOP TIMER 1
    TIM1_Base_Stop_IT();
  }
}