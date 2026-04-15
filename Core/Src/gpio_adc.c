#include "gpio_adc.h"
#include "timers.h"
#include <stdbool.h>
#include <stdint.h>

static volatile bool gpio_receiving = false;

void GPIO_start_receive(void) {
   // enable interrupt for GPIO pin 1 (PA1)
   HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
   gpio_receiving = true;
}

void GPIO_stop_receive(void) {
   // disable interrupt for GPIO pin 1 (PA1)
   HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
   gpio_receiving = false;
}

bool GPIO_is_receiving(void) { return gpio_receiving; }