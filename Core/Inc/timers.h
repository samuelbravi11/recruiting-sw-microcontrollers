#ifndef TIMERS_H
#define TIMERS_H

#include "main.h"
#include <stdbool.h>

extern volatile bool timer1_expired; // TODO

void TIM1_Base_Start_IT(void);
void TIM1_Base_Stop_IT(void);

void TIM3_Base_Start(void);
void TIM3_Base_Stop(void);

void TIM14_Base_Start_IT(void);
void TIM14_Base_Stop_IT(void);

void TIM16_Base_Start_IT(void);
void TIM16_Base_Stop_IT(void);

#endif