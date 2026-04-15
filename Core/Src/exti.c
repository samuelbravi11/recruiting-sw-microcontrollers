#include "gpio_adc.h"
#include "queue_event.h"
#include "timers.h"
#include <stdint.h>

bool volatile gpio_led_adc = 0;

// callback for rising edge on GPIO_PIN_ADC
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
   if (GPIO_Pin == GPIO_PIN_ADC) { // led for digital input of adc
      gpio_led_adc = 1;
      TIM1_Base_Start_IT();
   }
}

// callback for falling edge on GPIO_PIN_ADC
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
   static uint32_t last_btn_tick = 0;
   if (GPIO_Pin == GPIO_PIN_ADC) {
      gpio_led_adc = 0;
      TIM1_Base_Stop_IT();
   } else if (GPIO_Pin == GPIO_PIN_13) { // user button pressed
      // debounce of 200ms to not enqueue multiple button pressed event
      uint32_t now = HAL_GetTick();
      if (now - last_btn_tick >= 200) {
         last_btn_tick = now;
         enqueue_event(EVENT_BUTTON_PRESSED);
      }
   }
}