#include "timers.h"
#include "fsm.h"
#include "queue_event.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;

// TIMER 1 --> single shot timer to measure the time between rising and falling
// edge of GPIO pin
FSM_RET TIM1_Base_Start_IT(void) {
   __HAL_TIM_DISABLE(&htim1);
   __HAL_TIM_SET_COUNTER(&htim1, 0);
   __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
   FSM_RET status = HAL_TIM_Base_Start_IT(&htim1);
   if (status != FSM_OK) {
      set_error_message("TIM1 base start failed");
   }
   return status;
}
FSM_RET TIM1_Base_Stop_IT(void) {
   FSM_RET status = HAL_TIM_Base_Stop_IT(&htim1);
   if (status != FSM_OK) {
      set_error_message("TIM1 base stop failed");
   }
   __HAL_TIM_DISABLE(&htim1);
   __HAL_TIM_SET_COUNTER(&htim1, 0);
   __HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);
   return status;
}

// TIMER 3 --> used to trigger ADC conversion
FSM_RET TIM3_Base_Start(void) {
   FSM_RET status = HAL_TIM_Base_Start(&htim3);
   if (status != FSM_OK) {
      set_error_message("TIM3 base start failed");
   }
   return status;
}
FSM_RET TIM3_Base_Stop(void) {
   FSM_RET status = HAL_TIM_Base_Stop(&htim3);
   if (status != FSM_OK) {
      set_error_message("TIM3 base stop failed");
   }
   return status;
}

// TIMER 14 --> used with interrupt to toggle an LED every 1000 ms
// PWM mode is not used beacause it was necessary to use TIM1
FSM_RET TIM14_Base_Start_IT(void) {
   FSM_RET status = HAL_TIM_Base_Start_IT(&htim14);
   if (status != FSM_OK) {
      set_error_message("TIM14 base start failed");
   }
   return status;
}
FSM_RET TIM14_Base_Stop_IT(void) {
   FSM_RET status = HAL_TIM_Base_Stop_IT(&htim14);
   if (status != FSM_OK) {
      set_error_message("TIM14 base stop failed");
   }
   return status;
}

// TIMER 16 --> used with interrupt to toggle an LED every 200 ms
// PWM mode is not used beacause it was necessary to use TIM1
FSM_RET TIM16_Base_Start_IT(void) {
   FSM_RET status = HAL_TIM_Base_Start_IT(&htim16);
   if (status != FSM_OK) {
      set_error_message("TIM16 base start failed");
   }
   return status;
}
FSM_RET TIM16_Base_Stop_IT(void) {
   FSM_RET status = HAL_TIM_Base_Stop_IT(&htim16);
   if (status != FSM_OK) {
      set_error_message("TIM16 base stop failed");
   }
   return status;
}

// TIM1 single shot and TIM14 + TIM16 base timer callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
   // If timer 1 expires, it means that GPIO pin for ADC was hight for more than
   // 5 seconds
   if (htim->Instance == TIM1) {
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