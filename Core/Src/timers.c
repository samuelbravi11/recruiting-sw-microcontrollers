#include "timers.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

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
  timer1_expired = false;
}

// TIMER 3
void TIM3_Base_Start(void) {
  HAL_TIM_Base_Start(&htim3);
}
void TIM3_Base_Stop(void) {
  HAL_TIM_Base_Stop(&htim3);
}

// TIM1 callback single shot
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // If timer 1 expires, it means that GPIO pin for ADC was hight for more than 5 seconds
  if (htim->Instance == TIM1) {
    timer1_expired = true;
    HAL_TIM_Base_Stop_IT(&htim1);
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
  }
}