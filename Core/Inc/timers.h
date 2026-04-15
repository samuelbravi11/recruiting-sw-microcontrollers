#ifndef TIMERS_H
#define TIMERS_H

#include "fsm_types.h"
#include "main.h"
#include <stdbool.h>

FSM_RET TIM1_Base_Start_IT(void);
FSM_RET TIM1_Base_Stop_IT(void);

FSM_RET TIM3_Base_Start(void);
FSM_RET TIM3_Base_Stop(void);

FSM_RET TIM14_Base_Start_IT(void);
FSM_RET TIM14_Base_Stop_IT(void);

FSM_RET TIM16_Base_Start_IT(void);
FSM_RET TIM16_Base_Stop_IT(void);

#endif