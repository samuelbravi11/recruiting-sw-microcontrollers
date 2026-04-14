#include "gpio_adc.h"
#include "timers.h"
#include "core_cm0plus.h"
#include <stdint.h>
#include <stdbool.h>

void GPIO_start_receive(void) {
  // enable callback for GPIO_PIN_ADC
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void GPIO_stop_receive(void) {
  // disable callback for GPIO_PIN_ADC
  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
}

bool GPIO_is_receiving(void) {
  return (NVIC->ISER[0] & (1UL << EXTI4_15_IRQn)) != 0;
}