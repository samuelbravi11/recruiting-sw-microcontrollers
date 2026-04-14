#include "gpio_adc.h"
#include "timers.h"
#include "queue_event.h"
#include <stdint.h>

// callback for rising edge on GPIO_PIN_ADC
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_ADC) {
    // START TIMER 1
    TIM1_Base_Start_IT();
  } else if (GPIO_Pin == GPIO_PIN_13) { // user button pressed
    // toggle LED on PA5
    // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    // TODO
    enqueue_event(EVENT_BUTTON_PRESSED);
  }
}

// callback for falling edge on GPIO_PIN_ADC
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == GPIO_PIN_ADC) {
    // STOP TIMER 1
    TIM1_Base_Stop_IT();
  }
}