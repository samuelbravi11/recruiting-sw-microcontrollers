#include "timers.h"
#include "queue_event.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;

volatile bool timer1_expired = 0;

// TIMER 1 --> single shot timer to measure the time between rising and falling edge of GPIO pin
void TIM1_Base_Start_IT(void) {
  __HAL_TIM_DISABLE(&htim1);
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
  HAL_TIM_Base_Start_IT(&htim1);
}
void TIM1_Base_Stop_IT(void) {
  HAL_TIM_Base_Stop_IT(&htim1);
  __HAL_TIM_DISABLE(&htim1);
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
  // timer1_expired = false;
}


// TIMER 3 --> used to trigger ADC conversion
void TIM3_Base_Start(void) {
  HAL_TIM_Base_Start(&htim3);
}
void TIM3_Base_Stop(void) {
  HAL_TIM_Base_Stop(&htim3);
}


// TIMER 14 --> used with interrupt to toggle an LED every 1000 ms
// PWM mode is not used beacause it was necessary to use TIM1
void TIM14_Base_Start_IT(void) {
  HAL_TIM_Base_Start_IT(&htim14);
}
void TIM14_Base_Stop_IT(void) {
  HAL_TIM_Base_Stop_IT(&htim14);
}


// TIMER 16 --> used with interrupt to toggle an LED every 200 ms
// PWM mode is not used beacause it was necessary to use TIM1
void TIM16_Base_Start_IT(void) {
  HAL_TIM_Base_Start_IT(&htim16);
}
void TIM16_Base_Stop_IT(void) {
  HAL_TIM_Base_Stop_IT(&htim16);
}


// TIM1 single shot and TIM14 callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // If timer 1 expires, it means that GPIO pin for ADC was hight for more than 5 seconds
  if (htim->Instance == TIM1) {
    // timer1_expired = true;
    HAL_TIM_Base_Stop_IT(&htim1);
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
    // enqueue event to process it in the fsm dispatch
    enqueue_event(EVENT_TIMER_EXPIRED);
  } else if (htim->Instance == TIM14) {
    // toggle LED every 1000 ms
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  } else if (htim->Instance == TIM16) {
    // toggle LED every 200 ms
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
  }
}